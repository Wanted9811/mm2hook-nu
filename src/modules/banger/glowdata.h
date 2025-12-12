#pragma once
#include "mm2_common.h"
#include <modules\gfx\texture.h>

namespace MM2
{
    // Forward declarations
    class dgBangerGlowData;

    // External declarations


    // Class definitions
    class dgBangerGlowData
    {
    public:
        gfxTexture* RedGlow;
        gfxTexture* GlowShadow;
        gfxTexture* YelGlowShadow;
        Vector3 Offset;
        Vector4 Color;
        char Name[64];
        float Size;
        int Index;
        int FlatMode;
        Vector2 FlatSize;
        Vector3 FlatRotation;
        Vector3 RotationSpeed;
        int EnableFade;
        int FadeStyle;
        int FadeDirection;
        int InvFadeDirection;
        Vector2 FlashFreq;
        Vector2 FlashFreq2;
        int InvFlashTime;
        int FlashPerIndex;
        int EnableShadow;
        int CustomShadow;
        float ShadowSize;
        float ShadowIntensity;
    public:
        dgBangerGlowData();
    };

    // Lua initialization

}