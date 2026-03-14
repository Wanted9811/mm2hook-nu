#include "vehCarHandler.h"

using namespace MM2;

/*
    vehCarHandler
*/

static ConfigValue<bool> cfgVehicleDebug("VehicleDebug", "vehicleDebug", false);
static ConfigValue<bool> cfgEnableOutOfMapFix("OutOfMapFix", true);
static ConfigValue<bool> cfgEnableWaterSplashSound("WaterSplashSound", true);

void vehCarHandler::InitCar(LPCSTR vehName, int a2, int a3, bool a4, bool a5)
{
    auto car = reinterpret_cast<vehCar*>(this);
    Displayf("Initializing vehicle (\"%s\", %d, %d, %s, %s)", vehName, a2, a3, bool_str(a4), bool_str(a5));
    car->Init(vehName, a2, a3, a4, a5);
}

const phBound * vehCarHandler::GetModelBound(int a1)
{
    auto result = hook::Thunk<0x4648C0>::Call<const phBound *>(this, a1);

    if (result == NULL)
        Errorf(">>> COULD NOT RETRIEVE VEHICLE BOUND (%d) !!! <<<", a1);

    return result;
}

void vehCarHandler::InitCarAudio(LPCSTR a1, BOOL a2)
{
    auto car = reinterpret_cast<vehCar*>(this);
    char *vehName = car->GetCarDamage()->GetName();
    int flagsId = VehicleListPtr->GetVehicleInfo(vehName)->GetFlags();

    // debug if enabled
    if (cfgVehicleDebug.Get())
    {
        Displayf("Loading vehicle audio (\"%s\", %d)", a1, a2);
    }

    // automatic vehtypes system
    bool vehicleHasSiren = car->GetSiren() != nullptr && car->GetSiren()->GetLightCount() > 0;

    if ((vehicleHasSiren || flagsId & 8) && !vehCarAudioContainer::IsPolice(a1))
    {
        Displayf("%s has a lightbar, but is not in the vehtypes file. Adding it.", a1);
        string_buf<128> sirenBuffer("%s,ENDOFDATA", a1);
        vehCarAudioContainer::RegisterPoliceNames(NULL, (LPCSTR)sirenBuffer);
    }

    string_buf<128> semiDataName("%s_semidata", a1);
    bool semiDataExists = datAssetManager::Exists("aud\\cardata\\shared", (LPCSTR)semiDataName, "csv");
    if (semiDataExists && !vehCarAudioContainer::IsSemiOrBus(a1))
    {
        Displayf("%s has semidata, but is not in the vehtypes file. Adding it.", a1);
        string_buf<128> semiBuffer("%s,ENDOFDATA", a1);
        vehCarAudioContainer::RegisterSemiNames(NULL, (LPCSTR)semiBuffer);
    }

    // pass back to original function
    car->InitAudio(a1, a2);
}

void vehCarHandler::Mm1StyleTransmission()
{
    auto car = reinterpret_cast<vehCar*>(this);
    auto carsim = car->GetCarSim();
    auto engine = carsim->GetEngine();
    auto drivetrain = carsim->GetDrivetrain();
    auto transmission = carsim->GetTransmission();
    auto curDamage = car->GetCarDamage()->GetCurDamage();
    auto maxDamage = car->GetCarDamage()->GetMaxDamage();

    if (curDamage < maxDamage)
    {
        if (transmission->IsAuto())
        {
            if (carsim->GetSpeedMPH() >= 1.f && carsim->OnGround())
            {
                if (engine->GetThrottleInput() < 0.1f && transmission->GetGear() != 1)
                    engine->SetThrottleInput(0.1f);
            }

            // activate Handbrake if car goes under 1mph (P gear)
            if (carsim->GetSpeedMPH() < 1.f && engine->GetThrottleInput() < 0.1f)
                carsim->SetHandbrake(1.f);
        }
        else
        {
            if (carsim->GetBrake() < 0.1f && carsim->GetHandbrake() < 0.1f)
            {
                if (engine->GetThrottleInput() < 0.1f && transmission->GetGear() != 1)
                    engine->SetThrottleInput(0.1f);
            }
        }
    }

    // setting up this case for crash course
    // fixes ai cops and opponents have no brakes if they're damaged out
    if (curDamage >= maxDamage)
    {
        carsim->SetBrake(1.f);
    }

    // attach drive train if we just hit throttle
    // fixes the short delay that happens before the car moves
    if (engine->GetThrottleInput() > 0.f)
    {
        drivetrain->Attach();
    }
}

void vehCarHandler::Splash()
{
    auto car = reinterpret_cast<vehCar*>(this);
    float vehicleVelocity = car->GetModel()->GetVelocity().Mag();

    //trigger ColliderId 22 with velocity of vehicleMph
    auto impactAud = car->GetCarAudioContainerPtr()->GetAudImpactPtr();
    impactAud->Play(vehicleVelocity, 22);
}

void vehCarHandler::Zoink()
{
    //get required vars
    auto car = reinterpret_cast<vehCar*>(this);
    Vector3 carPos = car->GetModel()->GetPosition();

    if (car->IsPlayer())
    {
        Warningf("Player is out of the world, teleporting!");
        // tell the player "That didn't happen!"
        auto player = mmGameManager::Instance->getGame()->GetPlayer();
        player->GetHUD()->SetMessage(AngelReadString(29), 3.0f, 0);
    }

    // if we're in CNR, drop the gold!
    if (dgStatePack::Instance->GameMode == dgGameMode::CnR)
    {
        auto game = mmGameManager::Instance->getGame();
        hook::Thunk<0x425460>::ThisCall<void>(game); // mmMultiCR::DropThruCityHandler
    }

    // early exit
    auto AIMAP = aiMap::GetInstance();
    if (AIMAP == nullptr || AIMAP->GetIntersectionCount() == 0)
    {
        car->Reset();
        return;
    }

    // search for an intersection to teleport to
    float shortestDistance = 99999;
    int closestIntersection = -1;

    for (int is = 0; is < AIMAP->numIntersections; is++)
    {
        auto intersection = AIMAP->intersections[is];

        // avoid dummy intersections
        if (intersection->GetPathCount() == 0)
            continue;

        // check roads to see if this is a valid spawn point
        // valid == (!freeway && !alley)
        bool isValid = true;
        for (int i = 0; i < intersection->GetPathCount(); i++)
        {
            auto path = intersection->GetPath(i);
            ushort pathFlags = path->GetFlags();

            if (pathFlags & 4 || pathFlags & 2)
            {
                isValid = false;
                break;
            }
        }

        if (isValid)
        {
            float pDist = intersection->GetCenter().Dist(carPos);
            if (pDist < shortestDistance)
            {
                shortestDistance = pDist;
                closestIntersection = is;
            }
        }
    }

    // move car to the closest intersection if we can
    if (closestIntersection >= 0)
    {
        Vector3 originalResetPos = car->GetCarSim()->GetResetPos();
        car->GetCarSim()->SetResetPos(AIMAP->Intersection(closestIntersection)->GetCenter());
        car->Reset();
        car->GetCarSim()->SetResetPos(originalResetPos);
    }
}

void vehCarHandler::Update()
{
    auto car = reinterpret_cast<vehCar*>(this);
    auto siren = car->GetSiren();
    auto audio = car->GetCarAudioContainerPtr();
    auto model = car->GetModel();
    auto damage = car->GetCarDamage();
    auto engine = car->GetCarSim()->GetEngine();
    auto lightbar0 = model->GetGenBreakableMgr()->Get(1);
    auto lightbar1 = model->GetGenBreakableMgr()->Get(2);

    if ((lightbar0 != nullptr && !lightbar0->isAttached) ||
        (lightbar1 != nullptr && !lightbar1->isAttached))
    {
        if (siren != nullptr && siren->IsActive())
        {
            siren->SetActive(false);
            audio->StopSiren();
        }
    }

    if (vehCarModel::MM1StyleTransmission)
    {
        vehCarHandler::Mm1StyleTransmission();
    }

    //play splash sound if we just hit the water
    if (cfgEnableWaterSplashSound.Get())
    {
        bool splashState = car->GetSplash()->isActive();
        if (splashState && splashState != model->GetPrevSplashState())
        {
            Splash();
        }
        model->SetPrevSplashState(splashState);
    }

    // call original
    hook::Thunk<0x42C690>::Call<void>(this);

    //check if we're out of the level
    int carRoom = model->GetRoomId();
    if (carRoom == 0 && cfgEnableOutOfMapFix.Get())
    {
        Zoink();
    }
}

void vehCarHandler::PreUpdate()
{
    auto car = reinterpret_cast<vehCar*>(this);
    car->vehCar::PreUpdate();
}

void vehCarHandler::Install(void)
{
    InstallCallback("vehCar::InitAudio", "Enables debugging for vehicle initialization, and automatic vehtypes handling.",
        &InitCarAudio, {
            cb::call(0x55943A), // aiVehiclePhysics::Init
            cb::call(0x404090), // mmPlayer::Init
            cb::call(0x43C540), // mmNetObject::Init
        }
    );

    if (cfgVehicleDebug)
    {
        InstallCallback("vehCar::InitAudio", "Enables debugging for vehicle initialization.",
            &InitCarAudio, {
                cb::call(0x55943A), // aiVehiclePhysics::Init
                cb::call(0x404090), // mmPlayer::Init
                cb::call(0x43C540), // mmNetObject::Init
            }
        );

        InstallVTableHook("vehCarModel::GetBound",
            &GetModelBound, {
                0x5B2D14
            }
        );
    }

    InstallVTableHook("vehCar::PreUpdate",
        &PreUpdate, {
            0x5B0BB4,
        }
    );

    InstallVTableHook("vehCar::Update",
        &Update, {
            0x5B0BB8,
        }
    );

    if (vehCarModel::MM1StyleTransmission)
    {
        // deactivate auto Handbrake system
        InstallPatch({ 0xD8, 0x1D, 0x3C, 0x4, 0x5B, 0x0 }, {
            0x405C81,
        });
    }

    // fix tire skidmarks rendering being visible through buildings
    InstallPatch({
        0xEB,
        }, {
            0x42C5F3, // vehCar::DrawTracks
            0x42C66D, // vehCar::DrawTracks
        });
}

