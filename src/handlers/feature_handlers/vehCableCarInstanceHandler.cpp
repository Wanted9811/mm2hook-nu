#include "vehCableCarInstanceHandler.h"

using namespace MM2;

/*
    vehCableCarInstanceHandler
*/

void vehCableCarInstanceHandler::Draw(int lod)
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::Draw(lod);
}

void vehCableCarInstanceHandler::DrawShadow()
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::DrawShadow();
}

void vehCableCarInstanceHandler::DrawGlow()
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::DrawGlow();
}

void vehCableCarInstanceHandler::DrawReflected(float intensity)
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::DrawReflected(intensity);
}

void vehCableCarInstanceHandler::DrawReflectedParts(int lod)
{
    auto cableCar = reinterpret_cast<aiCableCarInstance*>(this);
    cableCar->aiCableCarInstance::DrawReflectedParts(lod);
}

bool vehCableCarInstanceHandler::BeginGeom(const char* a1, const char* a2, int a3)
{
    //We hook this to set flag 64 (shadow)
    auto inst = reinterpret_cast<lvlInstance*>(this);
    inst->SetFlag(lvlInstance::INST_SHADOW);

    //Call original
    return inst->BeginGeom(a1, a2, a3);
}

void vehCableCarInstanceHandler::Install()
{
    InstallVTableHook("vehCableCarInstanceHandler::Draw",
        &Draw, {
            0x5B5684,
        }
    );

    InstallVTableHook("vehCableCarInstanceHandler::DrawShadow",
        &DrawShadow, {
            0x5B5688,
        }
    );

    InstallVTableHook("vehCableCarInstanceHandler::DrawGlow",
        &DrawGlow, {
            0x5B5690,
        }
    );

    InstallVTableHook("vehCableCarInstanceHandler::DrawReflected",
        &DrawReflected, {
            0x5B5694,
        }
    );

    InstallVTableHook("vehCableCarInstanceHandler::DrawReflectedParts",
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

