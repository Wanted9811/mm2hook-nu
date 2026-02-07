#pragma once
#include <modules\model\static2.h>

namespace MM2
{
    // Forward declarations
    class mmDamage;

    // External declarations


    // Class definitions
    class mmDamage
    {
    private:
        int TextureCount;
        int VertexCount;
        modStatic* BodyModel;
        modStatic* CleanModel;
        modStatic* DamageModel;
        modShader* CleanShaders;
        modShader* DamageShaders;
        gfxTexture** DamageTextures;
        Vector3* VertexPositions;
    public:
        ANGEL_ALLOCATOR

        mmDamage();

        virtual ~mmDamage();

        void Init(modStatic* model, modShader* shaders, int shaderCount);

        void Reset(bool showDamage);

        void Apply(Vector3& impactPosition, float radius);

        modStatic* GetCleanModel();
        modStatic* GetDamageModel();
        modShader* GetCleanShaders();
        modShader* GetDamageShaders();
    };
}