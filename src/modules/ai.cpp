#include <modules\ai.h>
using namespace MM2;

/*
    aiVehicleManager
*/
declfield(aiVehicleManager::Instance)(0x6B31AC);
declfield(aiVehicleManager::SignalClock)(0x6B31F4);

/*
    aiTrafficLightInstance
*/

bool aiTrafficLightInstance::GlowsLoaded = false;
gfxTexture* aiTrafficLightInstance::RedGlowShadow = NULL;
gfxTexture* aiTrafficLightInstance::YelGlowShadow = NULL;
gfxTexture* aiTrafficLightInstance::GrnGlowShadow = NULL;