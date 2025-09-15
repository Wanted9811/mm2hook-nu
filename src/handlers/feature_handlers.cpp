#include "feature_handlers.h"
#include "lua_drawable.h"
#include <imgui\impl\imgui_impl_age.h>
#include <imgui\impl\imgui_impl_win32.h>

using namespace MM2;

static init_handler g_feature_handlers[] = {
    /*
        Initialize the really important handlers
    */

    CreateHandler<gfxPipelineHandler>("gfxPipeline"),
    CreateHandler<datCallbackExtensionHandler>("datCallback Extensions"),
    CreateHandler<InitMapHandler>("InitMapHandler"),

    /*
        Initialize the rest of the handlers
        Order doesn't really matter, just whatever looks neat
    */

    CreateHandler<asCullManagerHandler>("asCullManager"),

    CreateHandler<cityLevelHandler>("cityLevel"),
    CreateHandler<cityTimeWeatherLightingHandler>("cityTimeWeatherLighting"),

    CreateHandler<BridgeFerryHandler>("gizBridge/gizFerry"),

    CreateHandler<gizTrainHandler>("gizTrain"),
    CreateHandler<gizFerryHandler>("gizFerry"),
    CreateHandler<gizParkedCarMgrHandler>("gizParkedCarMgr"),

    CreateHandler<ImGuiFeatureHandler>("ImGuiFeatureHandler"),
    CreateHandler<FadeHandler>("FadeHandler"),

    CreateHandler<mmHudMapFeatureHandler>("mmHudMapFeatureHandler"),
    CreateHandler<mmIconsHandler>("mmIconsHandler"),
    CreateHandler<mmInterfaceFeatureHandler>("mmInterfaceFeatureHandler"),
    CreateHandler<mmDashViewHandler>("mmDashView"),
    CreateHandler<mmExternalViewHandler>("mmExternalView"),
    CreateHandler<mmDirSndHandler>("mmDirSnd"),
    CreateHandler<mmPlayerHandler>("mmPlayer"),
    CreateHandler<mmGameHandler>("mmGame"),
    CreateHandler<mmGameMusicDataHandler>("mmGameMusicData"),
    CreateHandler<mmGameMultiHandler>("mmGameMulti"),
    CreateHandler<aud3DObjectHandler>("aud3DObjectHandler"),
    CreateHandler<audControlHandler>("audControlHandler"),
    CreateHandler<Aud3DObjectManagerHandler>("Aud3DObjectManagerHandler"),
    CreateHandler<mmArrowHandler>("mmArrowHandler"),
    CreateHandler<mmSingleRaceHandler>("mmSingleRace"),
    CreateHandler<mmSingleRoamHandler>("mmSingleRoam"),

    CreateHandler<dgPhysManagerHandler>("dgPhysManager"),

    CreateHandler<dgBangerInstanceHandler>("dgBangerInstance"),
    CreateHandler<ltLensFlareHandler>("ltLensFlare"),

    CreateHandler<vehCarHandler>("vehCar"),
    CreateHandler<vehCarAudioContainerHandler>("vehCarAudioContainer"),
    CreateHandler<vehPoliceCarAudioHandler>("vehPoliceCarAudio"),
    CreateHandler<vehBreakableMgrHandler>("vehBreakableMgr"),
    CreateHandler<vehCarModelFeatureHandler>("vehCarModel"),
    CreateHandler<vehCarSimHandler>("vehCarSim"),
    CreateHandler<vehWheelHandler>("vehWheel"),
    CreateHandler<vehTrailerFeatureHandler>("vehTrailer"),
    CreateHandler<vehTrailerInstanceFeatureHandler>("vehTrailerInstance"),
    CreateHandler<vehCableCarInstanceHandler>("vehCableCarInstance"),
    CreateHandler<vehSirenHandler>("vehSiren"),
    CreateHandler<vehEngineFeatureHandler>("vehEngine"),

    CreateHandler<Dialog_NewPlayerHandler>("New player dialog"),

    CreateHandler<lvlHandler>("Propulator"),
    CreateHandler<lvlFixedAnyHandler>("lvlFixedAny"),
    CreateHandler<lvlInstanceFeatureHandler>("lvlInstance"),
    CreateHandler<sdlPage16Handler>("sdlPage16"),
    CreateHandler<vglHandler>("VGL drawing"),

    CreateHandler<StreamHandler>("Stream"),
    CreateHandler<TextureVariantHandler>("Texture variants"),

    CreateHandler<gettexHandler>("gettex"),

    CreateHandler<mmCityListHandler>("mmCityList"),

    CreateHandler<PUMainHandler>("PUMain"),
    CreateHandler<ViewDistanceHandler>("ViewDistanceHandler"),

    CreateHandler<fxTexelDamageHandler>("fxTexelDamage"),

    CreateHandler<pedestrianInstanceHandler>("pedestrianInstanceHandler"),

    CreateHandler<aiVehicleInstanceFeatureHandler>("aiVehicleInstance"),
    CreateHandler<aiVehiclePhysicsFeatureHandler>("aiVehiclePhysics"),
    CreateHandler<aiPoliceOfficerFeatureHandler>("aiPoliceOfficer"),

    CreateHandler<luaDrawableHandler>("luaDrawableHandler"),

    CreateHandler<modStaticHandler>("modStaticHandler")
};

#ifndef FEATURES_DECLARED
#define FEATURES_DECLARED
#endif
