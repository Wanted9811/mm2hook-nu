#include "mmPlayerHandler.h"

using namespace MM2;

static ConfigValue<bool> cfgAmbientSoundsWithMusic("AmbientSoundsWithMusic", true);
static ConfigValue<bool> cfgEnableModelVisibility("ModelVisibility", false);
static ConfigValue<bool> cfgEnableExplosionSound("ExplosionSound", true);
static ConfigValue<bool> cfgEnableMissingDashboardFix("MissingDashboardFix", true);
static ConfigValue<bool> cfgMm1StyleFlipOver("MM1StyleFlipOver", false);

/*
    mmPlayerHandler
*/

void mmPlayerHandler::PlayExplosion() {
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto car = player->GetCar();
    auto policeAudio = car->GetCarAudioContainerPtr()->GetPoliceCarAudioPtr();
    auto explosionSound = *getPtr<AudSoundBase*>(policeAudio, 0x138);
    if (explosionSound != nullptr) {
        if (!explosionSound->IsPlaying())
            explosionSound->PlayOnce(-1.f, -1.f);
    }
}

void mmPlayerHandler::UpdateHOG()
{
    if (ROOT->IsPaused())
        return;

    auto player = reinterpret_cast<mmPlayer*>(this);
    auto stuck = player->GetCar()->GetStuck();
    auto playerMtx = player->GetCar()->GetICS()->GetMatrix();
    
    // prevent vehStuck from doing the job of this function
    if (playerMtx.m11 <= 0.f)
        stuck->SetStuckTime(0.f);

    hook::Thunk<0x404920>::Call<void>(this); // call original UpdateHOG
}

void mmPlayerHandler::Update() {
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto car = player->GetCar();
    auto audio = car->GetCarAudioContainerPtr();
    auto siren = car->GetSiren();
    auto carsim = car->GetCarSim();
    auto engine = carsim->GetEngine();
    auto basename = player->GetCar()->GetCarDamage()->GetName();
    auto flagsId = VehicleListPtr->GetVehicleInfo(basename)->GetFlags();
    auto AIMAP = aiMap::GetInstance();

    //check if we're damaged out
    if (cfgEnableExplosionSound.Get()) {
        if (player->IsMaxDamaged()) {
            //turn off engine
            audio->SilenceEngine(1);
            engine->SetCurrentTorque(0.f);
            //play explosion sound if siren is activated
            if (siren != nullptr && siren->IsActive()) {
                siren->SetActive(false);
                audio->StopSiren();
                PlayExplosion();
            }
        }
        if (!player->IsMaxDamaged()) {
            audio->SilenceEngine(0);
        }
    }

    //check if dashboard model is missing
    if (cfgEnableMissingDashboardFix.Get() && MMSTATE->DashEnabled) {
        string_buf<80> buffer("%s_dash", basename);
        if (!datAssetManager::Exists("geometry", buffer, "pkg")) {
            player->GetHUD()->DeactivateDash();
            player->GetCamView()->SetCam(player->GetPovCam());
        }
    }

    //call original
    hook::Thunk<0x405760>::Call<void>(this);
}

void mmPlayerHandler::Reset() {
    // deactivate signal lights if they're active
    vehCarModel::HazardLightsState = false;
    vehCarModel::LeftSignalLightState = false;
    vehCarModel::RightSignalLightState = false;

    // call original
    hook::Thunk<0x404A60>::Call<void>(this);
}

void mmPlayerHandler::Install() {
    InstallVTableHook("mmPlayer::Update",
        &Update, {
            0x5B03BC
    });

    if (cfgAmbientSoundsWithMusic) {
        InstallPatch("Enables positional ambient sounds with music.", { 0x90, 0x90 }, {
            0x404044,
        });
    }

    InstallVTableHook("mmPlayer::Reset",
        &Reset, {
            0x5B03C0,
        }
    );

    if (cfgEnableModelVisibility.Get()) {
        InstallPatch("Enables vehicle's body visibility from the dashboard and bumper camera.", { 0x90, 0x90, 0x90 }, {
            0x405ED4,
        });
    }

    if (cfgMm1StyleFlipOver.Get())
    {
        //fix collision detection
        //TODO: fix this properly by setting the hasCollided flag!
        InstallPatch({ 0x8B, 0x81, 0xF4, 0x0, 0x0, 0x0 }, {
            0x40493F, // mmPlayer::UpdateHOG
            });

        InstallCallback("mmPlayer::UpdateHOG", "Wrap UpdateHOG to prevent flipping over while paused, and to disable stuck timer when upside down.",
            &UpdateHOG, {
                cb::call(0x405CAD),
            }
        );
    }
}

