#include "vehTrailerInstanceFeatureHandler.h"

using namespace MM2;

/*
    vehTrailerInstanceFeatureHandler
*/

void vehTrailerInstanceFeatureHandler::Draw(int lod) {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    inst->vehTrailerInstance::Draw(lod);
}

void vehTrailerInstanceFeatureHandler::DrawShadow() {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    inst->vehTrailerInstance::DrawShadow();
}

void vehTrailerInstanceFeatureHandler::DrawGlow() {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    inst->vehTrailerInstance::DrawGlow();
}

void vehTrailerInstanceFeatureHandler::DrawReflectedParts(int lod) {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    inst->vehTrailerInstance::DrawReflectedParts(lod);
}

void vehTrailerInstanceFeatureHandler::DrawReflected(float intensity) {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    inst->vehTrailerInstance::DrawReflected(intensity);
}

void vehTrailerInstanceFeatureHandler::Install() {
    InstallCallback("vehTrailerInstance::Init", "Use rewritten vehTrailerInstance init.",
        &vehTrailerInstance::Init, {
            cb::call(0x4D7350),
        }
    );

    InstallVTableHook("vehTrailerInstance::Draw",
        &Draw, {
            0x5B2FB0,
        }
    );

    InstallVTableHook("vehTrailerInstance::DrawShadow",
        &DrawShadow, {
            0x5B2FB4,
        }
    );

    InstallVTableHook("vehTrailerInstance::DrawGlow",
        &DrawGlow, {
            0x5B2FBC,
        }
    );

    InstallVTableHook("vehTrailerInstance::DrawReflected",
        &DrawReflected, {
            0x5B2FC0,
        }
    );

    InstallVTableHook("vehTrailerInstance::DrawReflectedParts",
        &DrawReflectedParts, {
            0x5B2FC4,
        }
    );
}