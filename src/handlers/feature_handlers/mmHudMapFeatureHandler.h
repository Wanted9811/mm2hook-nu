#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

using namespace MM2;

class mmHudMapFeatureHandler {
public:
    void DrawColoredTri(unsigned int color, const Matrix34& matrix);
    
    void DrawIcon(int iconColor, const Matrix34& matrix);
    void DrawOutlinedIcon(int iconColor, const Matrix34& matrix);
    void DrawOutlinedIcon(int iconColor, int iconOutlineColor, const Matrix34& matrix);

    void DrawPlayer();
    void DrawCops();
    void DrawOpponents();

    static void Install();
};

