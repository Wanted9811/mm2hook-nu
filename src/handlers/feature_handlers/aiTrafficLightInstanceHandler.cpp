#include "aiTrafficLightInstanceHandler.h"

using namespace MM2;

void aiTrafficLightInstanceHandler::DrawShadow()
{
	auto trafLight = reinterpret_cast<aiTrafficLightInstance*>(this);
	trafLight->aiTrafficLightInstance::DrawShadow();
}

void aiTrafficLightInstanceHandler::Install()
{
	InstallVTableHook("aiTrafficLightInstance::DrawShadow",
        &DrawShadow, {
            0x5B54DC
        }
    );
}