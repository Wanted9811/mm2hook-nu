#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class lvlFixedAnyHandler {
public:
    void Draw(int lod);
    void DrawReflectedParts(int lod);
    static void Install();
};