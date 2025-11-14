#include <events\dispatcher.h>
#include "lvlFixedAnyHandler.h"

using namespace MM2;

/*
    lvlFixedAnyHandler
*/

void lvlFixedAnyHandler::Draw(int lod)
{
    auto lvl = reinterpret_cast<lvlFixedAny*>(this);

    Matrix34 dummy = Matrix34();
    Matrix34 matrix = lvl->GetMatrix(dummy);
    gfxRenderState::SetWorldMatrix(matrix);

    modShader* shaders = lvl->GetShader(lvl->GetSubType());
    modStatic* model = lvl->GetGeom(lod, 0);
    if (model != nullptr)
        model->DrawNoGlass(shaders);

    if (model && (model->Flags & (vglCloudMapEnable.get() * 2)) != 0)
        model->DrawOrthoMapped(shaders, vglCloudMapTexture, 0.0078125f, matrix.GetRow(3), vglCloudMapEnable);
}

void lvlFixedAnyHandler::DrawReflectedParts(int lod)
{
    auto lvl = reinterpret_cast<lvlFixedAny*>(this);

    Matrix34 dummy = Matrix34();
    Matrix34 matrix = lvl->GetMatrix(dummy);
    gfxRenderState::SetWorldMatrix(matrix);

    modShader* shaders = lvl->GetShader(lvl->GetSubType());
    modStatic* model = lvl->GetGeom(lod, 0);
    if (model != nullptr)
        model->DrawGlass(shaders);
}

void lvlFixedAnyHandler::Install()
{
    InstallVTableHook("lvlFixedAny::Draw",
        &Draw, {
            0x5B1AB4,
            0x5B1B3C,
            0x5B1BA4,
            0x5B1C0C
        }
    );

    InstallVTableHook("lvlFixedAny::DrawReflectedParts",
        &DrawReflectedParts, {
            0x5B1AC8,
            0x5B1B50,
            0x5B1BB8,
            0x5B1C20
        }
    );
}