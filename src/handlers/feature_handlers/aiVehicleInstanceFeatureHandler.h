#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class aiVehicleInstanceFeatureHandler {
public:
    void Draw(int lod);
    void DrawGlow();
    void DrawShadow();
    void DrawReflectedParts(int lod);
    void DrawReflected(float intensity);
    void AddGeomHook(const char* pkgName, const char* name, int flags);
    void SetVariant(int a1);

    void VehicleSpline_DrawId();
    void Ambient_DrawId();

    static void Install();
};

