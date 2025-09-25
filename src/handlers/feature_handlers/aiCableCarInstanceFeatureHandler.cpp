#include "aiCableCarInstanceFeatureHandler.h"

using namespace MM2;

/*
    aiCableCarInstanceFeatureHandler
*/

void aiCableCarInstanceFeatureHandler::Draw(int lod)
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::Draw(lod);
}

void aiCableCarInstanceFeatureHandler::DrawShadow()
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::DrawShadow();
}

void aiCableCarInstanceFeatureHandler::DrawGlow()
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::DrawGlow();
}

void aiCableCarInstanceFeatureHandler::DrawReflected(float intensity)
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::DrawReflected(intensity);
}

void aiCableCarInstanceFeatureHandler::DrawReflectedParts(int lod)
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::DrawReflectedParts(lod);
}

bool aiCableCarInstanceFeatureHandler::BeginGeom(const char* a1, const char* a2, int a3)
{
    //We hook this to set flag 64 (shadow)
    auto inst = reinterpret_cast<lvlInstance*>(this);
    inst->SetFlag(lvlInstance::INST_SHADOW);

    //Call original
    return inst->BeginGeom(a1, a2, a3);
}

void aiCableCarInstanceFeatureHandler::Install()
{
    InstallVTableHook("aiCableCarInstance::Draw",
        &Draw, {
            0x5B5684,
        }
    );

    InstallVTableHook("aiCableCarInstance::DrawShadow",
        &DrawShadow, {
            0x5B5688,
        }
    );

    InstallVTableHook("aiCableCarInstance::DrawGlow",
        &DrawGlow, {
            0x5B5690,
        }
    );

    InstallVTableHook("aiCableCarInstance::DrawReflected",
        &DrawReflected, {
            0x5B5694,
        }
    );

    InstallVTableHook("aiCableCarInstance::DrawReflectedParts",
        &DrawReflectedParts, {
            0x5B5698,
        }
    );

    InstallCallback("aiCableCar::Init", "Hook BeginGeom to set instance shadowing flag.",
        &BeginGeom, {
            cb::call(0x53F8AA),
        }
    );
}

