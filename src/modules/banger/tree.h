#pragma once
#include <modules\banger.h>

namespace MM2
{
    // Forward declarations
    class dgTreeRenderer;

    // External declarations
    extern class dgBangerInstance;

    // Class definitions
    class dgTreeRenderer {
    private:
        static hook::Type<dgTreeRenderer*> sm_Instance;
    private:
        dgBangerInstance* Trees[200];
        int LOD[200];
        int TreeCount;
    public:
        static dgTreeRenderer* GetInstance();

        AGE_API dgTreeRenderer::dgTreeRenderer();
        AGE_API void dgTreeRenderer::AddTree(dgBangerInstance* tree, int lod);
        AGE_API void dgTreeRenderer::RenderTrees();
    };
    ASSERT_SIZEOF(dgTreeRenderer, 0x644);

    // Lua initialization

}