#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class aiCableCarInstanceFeatureHandler {
public:
    void Draw(int lod);
    void DrawShadow();
    void DrawGlow();
    void DrawReflected(float intensity);
    void DrawReflectedParts(int lod);
    bool BeginGeom(const char* a1, const char* a2, int a3);
    static void Install();
};

