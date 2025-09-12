#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class vehTrailerInstanceFeatureHandler {
public:
    void Draw(int lod);
    void DrawShadow();
    void DrawGlow();
    void DrawReflectedParts(int lod);
    void DrawReflected(float intensity);
    static void Install();
};