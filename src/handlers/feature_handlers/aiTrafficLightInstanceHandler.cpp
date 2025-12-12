#include <events\dispatcher.h>
#include "aiTrafficLightInstanceHandler.h"

using namespace MM2;

void aiTrafficLightInstanceHandler::Reset()
{
	aiTrafficLightInstance::RedGlowShadow = NULL;
	aiTrafficLightInstance::YelGlowShadow = NULL;
	aiTrafficLightInstance::GrnGlowShadow = NULL;
	aiTrafficLightInstance::GlowsLoaded = false;
}

void aiTrafficLightInstanceHandler::DrawShadow()
{
	auto trafLight = reinterpret_cast<aiTrafficLightInstance*>(this);
	trafLight->aiTrafficLightInstance::DrawShadow();
}

void aiTrafficLightInstanceHandler::Install()
{
	GameEventDispatcher::RegisterStateEndCallback(Reset);

	InstallVTableHook("aiTrafficLightInstance::DrawShadow",
        &DrawShadow, {
            0x5B54DC
        }
    );
}