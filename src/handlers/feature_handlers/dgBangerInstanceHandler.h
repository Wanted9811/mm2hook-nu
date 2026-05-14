#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class dgBangerInstanceHandler {
public:
    bool dgBangerInstance_BeginGeom(const char* a1, const char* a2, int a3); // Shadow flag hook
    bool Save();
    bool Load();

    void Draw(int lod);
    void DrawShadow();
    void DrawGlow();
	void DrawReflected(float intensity);
    void DrawReflectedParts(int lod);
    static void Reset();
    static void Install();
};

