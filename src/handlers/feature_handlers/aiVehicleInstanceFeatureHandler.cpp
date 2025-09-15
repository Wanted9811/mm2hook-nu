#include "aiVehicleInstanceFeatureHandler.h"

using namespace MM2;

static ConfigValue<int> cfgAmbientHeadlightStyle("AmbientHeadlightStyle", 0);

/*
    aiVehicleInstanceFeatureHandler
*/

void aiVehicleInstanceFeatureHandler::Draw(int lod)
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    inst->aiVehicleInstance::Draw(lod);
}

void aiVehicleInstanceFeatureHandler::DrawGlow()
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    inst->aiVehicleInstance::DrawGlow();
}

void aiVehicleInstanceFeatureHandler::DrawShadow()
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    inst->aiVehicleInstance::DrawShadow();
}

void aiVehicleInstanceFeatureHandler::DrawReflectedParts(int lod)
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    inst->aiVehicleInstance::DrawReflectedParts(lod);
}

void aiVehicleInstanceFeatureHandler::DrawReflected(float intensity)
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    inst->aiVehicleInstance::DrawReflected(intensity);
}

void aiVehicleInstanceFeatureHandler::AddGeomHook(const char* pkgName, const char* name, int flags)
{
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, name, flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "blight", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "plighton", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "plightoff", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "tslight0", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "tslight1", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "swhl0", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "swhl1", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "swhl2", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "swhl3", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "swhl4", flags);
    hook::Thunk<0x463BA0>::Call<int>(this, pkgName, "swhl5", flags);
}

void aiVehicleInstanceFeatureHandler::SetVariant(int a1)
{
    auto model = reinterpret_cast<aiVehicleInstance*>(this);
    model->aiVehicleInstance::SetVariant(a1);
}

void aiVehicleInstanceFeatureHandler::VehicleSpline_DrawId()
{
    reinterpret_cast<aiVehicleSpline*>(this)->aiVehicleSpline::DrawId();
}

void aiVehicleInstanceFeatureHandler::Ambient_DrawId()
{
    reinterpret_cast<aiVehicleAmbient*>(this)->aiVehicleAmbient::DrawId();
}

void aiVehicleInstanceFeatureHandler::Install()
{
    InstallCallback("aiVehicleInstance::aiVehicleInstance", "Adds brake light and pop-up lights geometries.",
        &AddGeomHook, {
            cb::call(0x551F2F),
        }
    );

    InstallVTableHook("aiVehicleInstance::Draw",
        &Draw, {
            0x5B5938
        }
    );

    aiVehicleInstance::AmbientHeadlightStyle = cfgAmbientHeadlightStyle.Get();
    InstallVTableHook("aiVehicleInstance::DrawGlow",
        &DrawGlow, {
            0x5B5944
        }
    );

    InstallVTableHook("aiVehicleInstance::DrawShadow",
        &DrawShadow, {
            0x5B593C
        }
    );

    InstallVTableHook("aiVehicleInstance::DrawReflectedParts",
        &DrawReflectedParts, {
            0x5B594C
        }
    );

    InstallVTableHook("aiVehicleInstance::DrawReflected",
        &DrawReflected, {
            0x5B5948
        }
    );

    InstallVTableHook("aiVehicleAmbient::DrawId",
        &Ambient_DrawId, {
            0x5B58E8
        }
    );

    InstallVTableHook("aiVehicleSpline::DrawId",
        &VehicleSpline_DrawId, {
            0x5B5B68
        }
    );

    InstallVTableHook("aiVehicleInstance::SetVariant",
        &SetVariant, {
            0x5B5920,
        }
    );
}

