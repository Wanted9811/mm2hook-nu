#include "aiVehicleInstanceFeatureHandler.h"

using namespace MM2;

static ConfigValue<int> cfgAmbientHeadlightStyle("AmbientHeadlightStyle", 0);

/*
    aiVehicleInstanceFeatureHandler
*/

aiVehicleInstance* aiVehicleInstanceFeatureHandler::Construct(aiVehicleSpline* spline, char* basename)
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    return new(inst) aiVehicleInstance(spline, basename);
}

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

void aiVehicleInstanceFeatureHandler::SetVariant(int a1)
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    inst->aiVehicleInstance::SetVariant(a1);
}

void* aiVehicleInstanceFeatureHandler::SizeOf()
{
    return hook::StaticThunk<0x463110>::Call<void*>(sizeof(aiVehicleInstance));
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
    InstallCallback("aiVehicleInstance::SizeOf", "Change size of aiVehicleInstance on aiVehicleSpline initialization.",
        &SizeOf, {
            cb::call(0x567F8B),
        }
    );
    
    InstallCallback("aiVehicleInstance::aiVehicleInstance", "Use our aiVehicleInstance Constructor.",
        &Construct, {
            cb::call(0x567FA8),
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

