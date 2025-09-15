#include "tree.h"

using namespace MM2;

declfield(dgTreeRenderer::sm_Instance)(0x627680);

dgTreeRenderer* dgTreeRenderer::GetInstance()
{
    return sm_Instance;
}

AGE_API dgTreeRenderer::dgTreeRenderer()
{
    hook::Thunk<0x442B00>::Call<void>(this);
}

AGE_API void dgTreeRenderer::AddTree(dgBangerInstance* tree, int lod)
{
    hook::Thunk<0x442B20>::Call<void>(this, tree, lod);
}

AGE_API void dgTreeRenderer::RenderTrees()
{
    hook::Thunk<0x442B50>::Call<void>(this);
}