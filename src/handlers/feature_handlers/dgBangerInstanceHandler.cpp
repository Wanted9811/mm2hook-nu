#include <events\dispatcher.h>
#include "dgBangerInstanceHandler.h"

using namespace MM2;

static ConfigValue<int> cfgPropShadows("3DShadows", 0);

/*
    dgBangerInstanceHandler
*/
hook::Type<int> sm_RefAlpha = 0x5C55F4;
hook::Type<gfxTexture*> glowTexture = 0x62767C;
gfxTexture* redGlowTexture;
bool glowLoaded = false;

void dgBangerInstanceHandler::Reset() {
    redGlowTexture = NULL;
    glowLoaded = false;
}

void dgBangerInstanceHandler::tglDrawParticle(const Vector3& position, float size, const Vector4& color)
{
    tglDrawParticleClipAdjusted(position, size, color);
}

void dgBangerInstanceHandler::Draw(int lod)
{
    auto banger = reinterpret_cast<dgBangerInstance*>(this);
    auto data = banger->GetData();

    int LOD = lod;
    if ((banger->GetFlags() & 4) != 0 && lod < 3)
        LOD = lod + 1;

    if ((data->BillFlags & 512) != 0 && dgTreeRenderer::GetInstance())
    {
        dgTreeRenderer::GetInstance()->AddTree(banger, LOD);
    }
    else
    {
        Matrix34 dummyMatrix = Matrix34();
        Matrix34 bangerMatrix = banger->GetMatrix(dummyMatrix);
        gfxRenderState::SetWorldMatrix(bangerMatrix);
        gfxRenderState::SetBlendSet(0, 0x80);

        auto model = banger->GetGeom(LOD, 0);
        if (model != nullptr)
        {
            if (data->BillFlags >= 0)
            {
                model->DrawNoGlass(banger->GetGeomBase()->pShaders[banger->GetVariant()]);
            }
            else
            {
                byte alphaRef = gfxRenderState::GetAlphaRef();
                gfxRenderState::SetAlphaRef((byte)sm_RefAlpha);
                gfxRenderState::SetTouchedMask(false);
                gfxRenderState::SetLighting(false);
                model->DrawNoGlass(banger->GetGeomBase()->pShaders[banger->GetVariant()]);
                gfxRenderState::SetTouchedMask(true);
                gfxRenderState::SetLighting(true);
                gfxRenderState::SetAlphaRef(alphaRef);
            }
        }
    }
}

void dgBangerInstanceHandler::DrawReflectedParts(int lod)
{
    auto banger = reinterpret_cast<dgBangerInstance*>(this);
    auto data = banger->GetData();

    int LOD = lod;
    if ((banger->GetFlags() & 4) != 0 && lod < 3)
        LOD = lod + 1;

    if (!((data->BillFlags & 512) != 0 && dgTreeRenderer::GetInstance()))
    {
        Matrix34 dummyMatrix = Matrix34();
        Matrix34 bangerMatrix = banger->GetMatrix(dummyMatrix);
        gfxRenderState::SetWorldMatrix(bangerMatrix);
        gfxRenderState::SetBlendSet(0, 0x80);

        auto model = banger->GetGeom(LOD, 0);
        if (model != nullptr)
        {
            if (data->BillFlags >= 0)
            {
                model->DrawGlass(banger->GetGeomBase()->pShaders[banger->GetVariant()]);
            }
            else
            {
                byte alphaRef = gfxRenderState::GetAlphaRef();
                gfxRenderState::SetAlphaRef((byte)sm_RefAlpha);
                gfxRenderState::SetTouchedMask(false);
                gfxRenderState::SetLighting(false);
                model->DrawGlass(banger->GetGeomBase()->pShaders[banger->GetVariant()]);
                gfxRenderState::SetTouchedMask(true);
                gfxRenderState::SetLighting(true);
                gfxRenderState::SetAlphaRef(alphaRef);
            }
        }
    }
}

void dgBangerInstanceHandler::DrawGlow()
{
    auto banger = reinterpret_cast<dgBangerInstance*>(this);

    //first time texture load
    if (!glowLoaded) {
        redGlowTexture = gfxGetTexture("s_red_glow", true);
        glowLoaded = true;
    }

    //prepare glow texture
    dgBangerData* data = banger->GetData();
    gfxTexture* lastTexture = (gfxTexture*)glowTexture;
    bool swappedTexture = false;

    if (!strcmp(data->GetName(), "sp_light_red_f") && lastTexture != NULL) {
        swappedTexture = true;
        glowTexture = redGlowTexture;
    }

    //draw glows
    ltLight::DrawGlowBegin();
    hook::Thunk<0x441840>::Call<void>(this); // call original
    ltLight::DrawGlowEnd();

    //reset glow texture
    if (swappedTexture) {
        glowTexture = lastTexture;
    }
}

void dgBangerInstanceHandler::DrawShadow()
{
    auto banger = reinterpret_cast<dgBangerInstance*>(this);
    auto timeWeather = cityLevel::GetCurrentLighting();

    if (MMSTATE->TimeOfDay == 3 || lvlLevel::GetSingleton()->GetRoomInfo(banger->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
        return;

    bool prevLighting = gfxRenderState::SetLighting(true);

    //invert model faces
    auto prevCullMode = gfxRenderState::GetCullMode();
    gfxRenderState::SetCullMode(D3DCULL_CCW);

    auto shaders = banger->GetShader(banger->GetVariant());
    modStatic* model = banger->GetGeomBase(0)->GetHighestLOD();

    if (model != nullptr)
    {
        Matrix34 shadowMatrix, dummyMatrix;
        Matrix34 bangerMatrix = banger->GetMatrix(dummyMatrix);
        if (lvlInstance::ComputeShadowProjectionMatrix(shadowMatrix, banger->GetRoomId(), timeWeather->KeyPitch, timeWeather->KeyHeading, bangerMatrix, banger))
        {
            gfxRenderState::SetWorldMatrix(shadowMatrix);
            model->DrawShadowed(shaders, ComputeShadowIntensity(timeWeather->KeyColor));
        }
    }

    //revert model faces back
    gfxRenderState::SetCullMode(prevCullMode);
    gfxRenderState::SetLighting(prevLighting);
}

bool dgBangerInstanceHandler::dgBangerInstance_BeginGeom(const char* a1, const char* a2, int a3)
{
    //We hook this to set flag 64 (shadow)
    auto inst = reinterpret_cast<lvlInstance*>(this);
    inst->SetFlag(lvlInstance::INST_SHADOW);

    //Call original
    return inst->BeginGeom(a1, a2, a3);
}

void dgBangerInstanceHandler::Install()
{
    InstallCallback("aiTrafficLightInstance::DrawGlow", "Make traffic light banger lights double sided.",
        &DrawGlow, {
            cb::call(0x53CCFD),
        }
    );

    InstallCallback("dgBangerInstance::DrawGlow", "Use custom tglDrawParticle.",
        &tglDrawParticle, {
            cb::call(0x441966),
        }
    );

    InstallVTableHook("dgBangerInstance::Draw",
        &Draw, {
            0x5B14C0,
            0x5B1538,
            0x5B15E4,
            0x5B5700,
            0x5B6100
        }
    );

    InstallVTableHook("dgBangerInstance::DrawReflectedParts",
        &DrawReflectedParts, {
            0x5B14D4,
            0x5B154C,
            0x5B15F8,
            0x5B1668,
            0x5B54EC,
            0x5B5714,
            0x5B57D8,
            0x5B5FCC,
            0x5B6114,
            0x5B61C0
        }
    );

    // makes banger glows double sided
    InstallVTableHook("dgBangerInstance::DrawGlow",
        &DrawGlow, {
            0x5B14CC,
            0x5B1544,
            0x5B15F0,
            0x5B570C,
            0x5B57D0,
            0x5B5FC4,
            0x5B610C,
            0x5B61B8
        }
    );

    GameEventDispatcher::RegisterStateEndCallback(Reset);

    if (cfgPropShadows.Get())
    {
        InstallVTableHook("dgBangerInstance::DrawShadow",
            &DrawShadow, {
                0x5B14C4,
                0x5B153C,
                0x5B15E8,
                0x5B1658,
                0x5B54DC,
                0x5B5704,
                0x5B57C8,
                0x5B5FBC,
                0x5B6104,
                0x5B61B0
            }
        );

        InstallCallback("dgBangerInstance::Init", "Hook BeginGeom to set instance shadowing flag.",
            &dgBangerInstance_BeginGeom, {
                cb::call(0x53C7DE), // aiTrafficLightInstance::Init
                cb::call(0x441C86), // dgUnhitBangerInstance::Init
            }
        );

        // increases the maximum distance from 5 to 10 meters to enable shadow rendering for very tall props 
        InstallPatch({ 0xD8, 0x25, 0x28, 0x4, 0x5B, 0x0 }, {
            0x464501
            });
    }
}

