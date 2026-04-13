#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class aiVehicleInstanceFeatureHandler {
public:
    MM2::aiVehicleInstance* Construct(MM2::aiVehicleSpline* spline, char* basename);
    void Draw(int lod);
    void DrawGlow();
    void DrawShadow();
    void DrawReflectedParts(int lod);
    void DrawReflected(float intensity);
    void SetVariant(int a1);

    void VehicleSpline_DrawId();
    void Ambient_DrawId();

    void* SizeOf();
    static void Install();
};

