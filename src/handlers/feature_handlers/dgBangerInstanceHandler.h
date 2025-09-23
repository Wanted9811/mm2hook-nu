#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class dgBangerInstanceHandler {
public:
    bool dgBangerInstance_BeginGeom(const char* a1, const char* a2, int a3); // Shadow flag hook

    void Draw(int lod);
    void DrawReflectedParts(int lod);
    void DrawGlow();
    void DrawShadow();
    static void Reset();
    static void Install();
};

