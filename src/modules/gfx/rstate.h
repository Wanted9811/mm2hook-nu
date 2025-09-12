#pragma once
#include <mm2_common.h>
#include "texture.h"
#include "material.h"
#include "stats.h"

namespace MM2
{
    // Forward declarations
    class gfxRenderStateData;
    class gfxRenderState;
    enum gfxDrawMode;

    // External declarations

    // Class definitions
    // same values as D3DPRIMITIVETYPE
    enum gfxDrawMode {
        DRAWMODE_POINTLIST      = 1,
        DRAWMODE_LINELIST       = 2,
        DRAWMODE_LINESTRIP      = 3,
        DRAWMODE_TRIANGLELIST   = 4,
        DRAWMODE_TRIANGLESTRIP  = 5,
        DRAWMODE_TRIANGLEFAN    = 6,
    };

    class gfxRenderStateData {
        /*
            Don't edit this unless you know what you're doing!
        */
    public:
        bool Clipping;
        bool Lighting;

        byte SrcBlend;              /* D3DBLEND */
        byte DestBlend;             /* D3DBLEND */
        byte VertexBlend;           /* D3DVERTEXBLENDFLAGS */

        int TextureArg;             /* D3DTA_* member */
        int BlendSet;               /* gfxBlendSet */

        float FogStart;             /* D3DRENDERSTATE_FOGSTART */
        float FogEnd;               /* D3DRENDERSTATE_FOGEND */

        byte ZEnable;               /* D3DZBUFFERTYPE */
        byte ShadeMode;             /* D3DSHADEMODE */
        bool ZWriteEnable;
        byte CullMode;              /* D3DCULL */
        byte ZFunc;                 /* D3DCMPFUNC */
        byte AlphaRef;              /* 0x0 - 0xFF */
        byte AlphaFunc;             /* D3DCMPFUNC */
        bool DitherEnable;
        bool AlphaEnable;           /* ALPHABLENDENABLE and ALPHATESTENABLE */
        bool TexturePerspective;
        byte FillMode;              /* D3DSHADEMODE */
        byte ZBias;                 /* 0 - 16 */
        byte AntiAlias;             /* D3DANTIALIASMODE */
        byte FogVertexMode;         /* D3DFOGMODE */

        byte AddrU[2];              /* D3DTSS_ADDRESSU States */
        byte AddrV[2];              /* D3DTSS_ADDRESSV States */
        
        int unk_2C;                 /* unused */
        
        ColorARGB Ambient;

        ColorARGB FogColor;
        float FogDensity;

        byte ColorOp1;
        byte ColorOp2;              /* educated guess -- unused */

        bool SpecularEnable;
        bool FogEnable;
        bool NormalizeNormals;
        bool RangeFogEnable;

        gfxTexture *Texture[2];
        gfxMaterial *Material;
    };

    declhook(0x6854A0, _TypeProxy<gfxRenderStateData>, LASTRSTATE);
    declhook(0x6856A0, _TypeProxy<gfxRenderState>, RSTATE);
    ASSERT_SIZEOF(gfxRenderStateData, 0x50);

    class gfxRenderState {
    public:
        gfxRenderStateData Data;
        gfxMaterial Material;

    private:
        /*
            m_Touched:
            0x01 | State Changed
            0x02 | Texture Changed
            0x04 | Material Changed
            0x08 | Transform Changed
            0x80 | Should Regenerate (gfxRenderState::Regenerate())
        */
        static hook::Type<int> m_Touched;
        static hook::Type<int> m_TouchedMask;
        static hook::Type<int[2]> m_TouchedMasks;

    private:
        static bool sm_EnableTextures;

        static hook::TypeProxy<Matrix44> sm_Camera;
        static hook::TypeProxy<Matrix44> sm_World;

        static hook::TypeProxy<Matrix44> sm_Composite;
        static hook::TypeProxy<Matrix44> sm_FullComposite;

        static hook::TypeProxy<Matrix44> sm_View;
        static hook::TypeProxy<Matrix44> sm_Modelview;

        static hook::TypeProxy<Matrix44> ScaleZ;
    public:
        static void Regenerate()                        { hook::StaticThunk<0x4B2820>::Call<void>(); }
        static void SetCard(Vector3 const & pos)        { hook::StaticThunk<0x4B2680>::Call<void>(&pos); }
        static void SetCamera(Matrix44 const & mtx)     { hook::StaticThunk<0x4B2A20>::Call<void>(&mtx); }
        static void SetCamera(Matrix34 const & mtx)     { hook::StaticThunk<0x4B2970>::Call<void>(&mtx); }
        static void SetCameraFull(Matrix34 const & mtx) { hook::StaticThunk<0x4B2B50>::Call<void>(&mtx); }
        static void SetView(Matrix34 const & mtx)       { hook::StaticThunk<0x4B2A80>::Call<void>(&mtx); }
        static void DisableAllLights()                  { hook::StaticThunk<0x4B22D0>::Call<void>(); }
    public:
        static void FlushMasked()
        {
            if ((gfxRenderState::m_TouchedMask & gfxRenderState::m_Touched) != 0)
                RSTATE->DoFlush(&LASTRSTATE);
        }

        static bool Touched(int flags)
        {
            return gfxRenderState::m_Touched & flags;
        }

        static void SetTouched(int flags)
        {
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | flags;
        }

        static void SetTouchedMask(bool lightingState)
        {
            gfxRenderState::m_TouchedMask = gfxRenderState::m_TouchedMasks[lightingState];
        }

        static void SetBlendSet(int a, int b)
        {
            hook::Thunk<0x4B2350>::ThisCall<void>(&RSTATE, a, b);
        }

        static gfxMaterial* SetMaterial(gfxMaterial* material)
        {
            auto original = (&RSTATE->Data)->Material;
            if (original != material) {
                (&RSTATE->Data)->Material = material;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x04;
            }
            return original;
        }

        static gfxTexture* SetTexture(int textureIndex, gfxTexture* texture)
        {
            assert(textureIndex == 0 || textureIndex == 1);

            if(!gfxRenderState::sm_EnableTextures)
                texture = nullptr;
            
            auto original = (&RSTATE->Data)->Texture[textureIndex];
            if (original != texture) {
                gfxTextureChanges++;
                (&RSTATE->Data)->Texture[textureIndex] = texture;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x02;
            }
            return original;
        }

        static D3DCULL SetCullMode(D3DCULL cullMode)
        {
            auto original = static_cast<D3DCULL>((&RSTATE->Data)->CullMode);
            if (original != cullMode) {
                (&RSTATE->Data)->CullMode = static_cast<byte>(cullMode);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static byte SetAlphaRef(byte alphaRef) 
        {
            auto original = (&RSTATE->Data)->AlphaRef;
            if (original != alphaRef) {
                (&RSTATE->Data)->AlphaRef = alphaRef;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static bool SetLighting(bool enabled)
        {
            auto original = (&RSTATE->Data)->Lighting;
            if (original != enabled) {
                (&RSTATE->Data)->Lighting = enabled;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static bool GetZWriteEnabled()
        {
            return (&RSTATE->Data)->ZWriteEnable;
        }

        static bool SetZWriteEnabled(bool enabled)
        {
            auto original = (&RSTATE->Data)->ZWriteEnable;
            if (original != enabled) {
                (&RSTATE->Data)->ZWriteEnable = enabled;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }
        
        static unsigned char SetZBias(unsigned char bias)
        {
            auto original = (&RSTATE->Data)->ZBias;
            if (original != bias) {
                (&RSTATE->Data)->ZBias = bias;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static D3DZBUFFERTYPE SetZEnabled(D3DZBUFFERTYPE enabled)
        {
            auto original = static_cast<D3DZBUFFERTYPE>((&RSTATE->Data)->ZEnable);
            if (original != enabled) {
                (&RSTATE->Data)->ZEnable = static_cast<byte>(enabled);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static bool GetAlphaEnabled()
        {
            return (&RSTATE->Data)->AlphaEnable;
        }

        static bool SetAlphaEnabled(bool enabled)
        {
            auto original = (&RSTATE->Data)->AlphaEnable;
            if (original != enabled) {
                (&RSTATE->Data)->AlphaEnable = enabled;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static bool SetClipping(bool enabled)
        {
            auto original = (&RSTATE->Data)->Clipping;
            if (original != enabled) {
                (&RSTATE->Data)->Clipping = enabled;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static bool SetFogEnabled(bool enabled)
        {
            auto original = (&RSTATE->Data)->FogEnable;
            if (original != enabled) {
                (&RSTATE->Data)->FogEnable = enabled;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static D3DCMPFUNC SetZFunc(D3DCMPFUNC func)
        {
            auto original = static_cast<D3DCMPFUNC>((&RSTATE->Data)->ZFunc);
            if (original != func) {
                (&RSTATE->Data)->ZFunc = static_cast<byte>(func);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static D3DCMPFUNC SetAlphaFunc(D3DCMPFUNC func) 
        {
            auto original = static_cast<D3DCMPFUNC>((&RSTATE->Data)->AlphaFunc);
            if (original != func) {
                (&RSTATE->Data)->AlphaFunc = static_cast<byte>(func);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static float SetFogStart(float value)
        {
            auto original = (&RSTATE->Data)->FogStart;
            if (original != value) {
                (&RSTATE->Data)->FogStart = value;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static float SetFogEnd(float value)
        {
            auto original = (&RSTATE->Data)->FogEnd;
            if (original != value) {
                (&RSTATE->Data)->FogEnd = value;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static ColorARGB SetFogColor(ColorARGB color)
        {
            auto original = (&RSTATE->Data)->FogColor;
            if (original != color) {
                (&RSTATE->Data)->FogColor = color;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static D3DFOGMODE SetFogVertexMode(D3DFOGMODE func)
        {
            auto original = static_cast<D3DFOGMODE>((&RSTATE->Data)->FogVertexMode);
            if (original != func) {
                (&RSTATE->Data)->FogVertexMode = static_cast<byte>(func);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x01;
            }
            return original;
        }

        static D3DCULL GetCullMode()
        {
            return static_cast<D3DCULL>((&RSTATE->Data)->CullMode);
        }

        static byte GetAlphaRef()
        {
            return (&RSTATE->Data)->AlphaRef;
        }

        static byte GetAlphaFunc()
        {
            return (&RSTATE->Data)->AlphaFunc;
        }

        static Matrix44 GetCameraMatrix()
        {
            return gfxRenderState::sm_Camera;
        }

        static Matrix44 GetViewMatrix()
        {
            return gfxRenderState::sm_View;
        }

        static Matrix44 GetModelViewMatrix()
        {
            return gfxRenderState::sm_Modelview;
        }

        static Matrix44 GetWorldMatrix()
        {
            return gfxRenderState::sm_World;
        }
        
        static Matrix44 GetFullComposite()
        {
            return gfxRenderState::sm_FullComposite;
        }

        static void SetWorldMatrix(const Matrix44& matrix)
        {
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
            memcpy(&gfxRenderState::sm_World, &matrix, sizeof(Matrix44));
        }

        static void SetWorldMatrix(const Matrix34& matrix)
        {
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
            Matrix44::Convert(gfxRenderState::sm_World, matrix);
        }

        static void SetView(const Matrix44& matrix)
        {
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x80;
            memcpy(&gfxRenderState::sm_View, &matrix, sizeof(Matrix44));
        }

        static byte SetFillMode(byte fillMode)
        {
            auto original = (&RSTATE->Data)->FillMode;
            if (original != fillMode) {
                (&RSTATE->Data)->FillMode = fillMode;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
            }
            return original;
        }

        void DoFlush(gfxRenderStateData* data)
        {
            hook::Thunk<0x4B4C40>::Call<void>(this, data);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<gfxRenderState>("gfxRenderState")
                .addStaticVariable("EnableTextures", &sm_EnableTextures)
                .addStaticProperty("WorldMatrix", &GetWorldMatrix, static_cast<void(*)(const Matrix44 &)>(&gfxRenderState::SetWorldMatrix))
                .addStaticProperty("ViewMatrix", &GetViewMatrix, static_cast<void(*)(const Matrix44&)>(&gfxRenderState::SetView))
                .addStaticProperty("CameraMatrix", &GetCameraMatrix, static_cast<void(*)(const Matrix44&)>(&gfxRenderState::SetCamera))
                .endClass();
        }
    };
    ASSERT_SIZEOF(gfxRenderState, 0x98);
}