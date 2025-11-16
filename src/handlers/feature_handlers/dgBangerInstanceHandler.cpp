#include <events\dispatcher.h>
#include "dgBangerInstanceHandler.h"

using namespace MM2;

static ConfigValue<int> cfgPropShadows("3DShadows", 0);
static ConfigValue<bool> cfgLightShadows("LightShadows", true);

/*
    dgBangerInstanceHandler
*/

void dgBangerInstanceHandler::Reset()
{
    dgBangerInstance::RedGlowTexture = NULL;
    dgBangerInstance::GlowLoaded = false;
}

void dgBangerInstanceHandler::Draw(int lod)
{
    auto banger = reinterpret_cast<dgBangerInstance*>(this);
    banger->dgBangerInstance::Draw(lod);
}

void dgBangerInstanceHandler::DrawReflectedParts(int lod)
{
    auto banger = reinterpret_cast<dgBangerInstance*>(this);
    banger->dgBangerInstance::DrawReflectedParts(lod);
}

void dgBangerInstanceHandler::DrawGlow()
{
    auto banger = reinterpret_cast<dgBangerInstance*>(this);
    banger->dgBangerInstance::DrawGlow();
}

void dgBangerInstanceHandler::DrawShadow()
{
    auto banger = reinterpret_cast<dgBangerInstance*>(this);
    banger->dgBangerInstance::DrawShadow();
}

bool dgBangerInstanceHandler::Save()
{
    auto data = reinterpret_cast<dgBangerData*>(this);
    return data->dgBangerData::Save();
}

bool dgBangerInstanceHandler::Load()
{
    auto data = reinterpret_cast<dgBangerData*>(this);
    return data->dgBangerData::Load();
}

bool dgBangerInstanceHandler::dgBangerInstance_BeginGeom(const char* a1, const char* a2, int a3)
{
    //We hook this to set flag 64 (shadow)
    auto inst = reinterpret_cast<lvlInstance*>(this);
    inst->SetFlag(lvlInstance::INST_SHADOW);

    //Call original
    return inst->BeginGeom(a1, a2, a3);
}

void dgBangerInstanceHandler::Install()
{
    dgBangerInstance::PropShadows = cfgPropShadows.Get();
    dgBangerInstance::LightShadows = cfgLightShadows.Get();
    InstallCallback("aiTrafficLightInstance::DrawGlow", "Make traffic light banger lights double sided.",
        &DrawGlow, {
            cb::call(0x53CCFD),
        }
    );

    InstallVTableHook("dgBangerInstance::Draw",
        &Draw, {
            0x5B14C0,
            0x5B1538,
            0x5B15E4,
            0x5B5700,
            0x5B6100
        }
    );

    InstallVTableHook("dgBangerInstance::DrawReflectedParts",
        &DrawReflectedParts, {
            0x5B14D4,
            0x5B154C,
            0x5B15F8,
            0x5B1668,
            0x5B54EC,
            0x5B5714,
            0x5B57D8,
            0x5B5FCC,
            0x5B6114,
            0x5B61C0
        }
    );

    // makes banger glows double sided
    InstallVTableHook("dgBangerInstance::DrawGlow",
        &DrawGlow, {
            0x5B14CC,
            0x5B1544,
            0x5B15F0,
            0x5B570C,
            0x5B57D0,
            0x5B5FC4,
            0x5B610C,
            0x5B61B8
        }
    );

    InstallVTableHook("dgBangerData::Save",
        &Save, {
            0x5B1484
        }
    );

    InstallVTableHook("dgBangerData::Load",
        &Load, {
            0x5B1488
        }
    );

    GameEventDispatcher::RegisterStateEndCallback(Reset);

    InstallVTableHook("dgBangerInstance::DrawShadow",
        &DrawShadow, {
            0x5B14C4,
            0x5B153C,
            0x5B15E8,
            0x5B1658,
            0x5B5704,
            0x5B57C8,
            0x5B5FBC,
            0x5B6104,
            0x5B61B0
        }
    );

    InstallCallback("dgBangerInstance::Init", "Hook BeginGeom to set instance shadowing flag.",
        &dgBangerInstance_BeginGeom, {
            cb::call(0x53C7DE), // aiTrafficLightInstance::Init
            cb::call(0x441C86), // dgUnhitBangerInstance::Init
        }
    );
}

