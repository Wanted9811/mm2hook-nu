#pragma once
#include "banger.h"
#include <modules\lights\light.h>
#include <modules\city\citylevel.h>
#include <modules\mmcityinfo\state.h>

using namespace MM2;

/*
    dgBangerInstance
*/

dgBangerData* dgBangerInstance::GetData() const                                  { return hook::Thunk<0x441AB0>::Call<dgBangerData*>(this); }

int dgBangerInstance::GetVariant() const
{
    return (unsigned short)this->DataPack >> 12;
}

void dgBangerInstance::SetBangerType(unsigned short type)
{
    this->DataPack ^= (this->DataPack ^ type) & 0xFFF;
}

unsigned short dgBangerInstance::GetBangerType() const
{
    return this->DataPack & 0xFFF;
}

void dgBangerInstance::DrawGlowShadow()
{
    //get glow shadow textures
    gfxTexture* ltGlowShadow = gfxGetTexture("lt_glow_shadow", true);
    gfxTexture* yelGlowShadow = gfxGetTexture("lt_yel_glow_shadow", true);

    //no need to draw glow shadows if they don't exist
    if (ltGlowShadow == NULL && yelGlowShadow == NULL)
        return;
    if (DefaultGlowTexture.get() == NULL)
        return;

    //get matrix
    Matrix34 dummyMatrix, shadowMatrix, lightMatrix = Matrix34();
    Matrix34 bangerMatrix = this->GetMatrix(dummyMatrix);

    //get data
    auto data = this->GetData();

    //draw light shadows
    gfxRenderState::SetFogEnabled(false);
    gfxRenderState::SetBlendSet(7);
    rglWorldIdentity();
    vglBeginBatch();
    for (int i = 0; i < data->NumGlows; i++)
    {
        auto glowData = &data->GlowDatas[i];
        if (!glowData->EnableShadow)
            continue;

        glowData->CustomShadow ? vglBindTexture(ltGlowShadow) : vglBindTexture(yelGlowShadow);

        Vector3 position = bangerMatrix.Transform(glowData->Offset);
        lightMatrix.Set(bangerMatrix);
        lightMatrix.SetRow(3, position);

        if (lvlInstance::ComputeShadowMatrix(shadowMatrix, this->GetRoomId(), lightMatrix))
        {
            shadowMatrix.m31 += 0.15f; //sidewalk height
            tglDrawCustomShadowedParticle(shadowMatrix, position, glowData, this);
        }
    }
    vglEndBatch();
    gfxRenderState::SetBlendSet(0);
    gfxRenderState::SetFogEnabled(!useSoftware);
}

/*
    lvlInstance virtuals
*/

AGE_API void dgBangerInstance::SetVariant(int variant)                           { hook::Thunk<0x442A10>::Call<void>(this, variant); }
AGE_API dgPhysEntity* dgBangerInstance::GetEntity()                              { return hook::Thunk<0x441AD0>::Call<dgPhysEntity*>(this); }
AGE_API dgPhysEntity* dgBangerInstance::AttachEntity()                           { return hook::Thunk<0x441AE0>::Call<dgPhysEntity*>(this); }
AGE_API Vector3 const & dgBangerInstance::GetVelocity()                          { return hook::Thunk<0x441B20>::Call<Vector3 const &>(this); }

AGE_API void dgBangerInstance::Draw(int lod)
{
    auto data = this->GetData();

    int LOD = lod;
    if ((this->GetFlags() & 4) != 0 && lod < 3)
        LOD = lod + 1;

    if ((data->BillFlags & 512) != 0 && dgTreeRenderer::GetInstance())
    {
        dgTreeRenderer::GetInstance()->AddTree(this, LOD);
    }
    else
    {
        Matrix34 dummyMatrix = Matrix34();
        Matrix34 bangerMatrix = this->GetMatrix(dummyMatrix);
        gfxRenderState::SetWorldMatrix(bangerMatrix);
        gfxRenderState::SetBlendSet(0, 0x80);

        auto model = this->GetGeom(LOD, 0);
        if (model != nullptr)
        {
            if (data->BillFlags >= 0)
            {
                model->DrawNoGlass(this->GetGeomBase()->pShaders[this->GetVariant()]);
            }
            else
            {
                byte alphaRef = gfxRenderState::GetAlphaRef();
                gfxRenderState::SetAlphaRef((byte)sm_RefAlpha.get());
                gfxRenderState::SetTouchedMask(false);
                gfxRenderState::SetLighting(false);
                model->DrawNoGlass(this->GetGeomBase()->pShaders[this->GetVariant()]);
                gfxRenderState::SetTouchedMask(true);
                gfxRenderState::SetLighting(true);
                gfxRenderState::SetAlphaRef(alphaRef);
            }
        }
    }
}

AGE_API void dgBangerInstance::DrawShadow()
{
    if (dgBangerInstance::LightShadows)
        DrawGlowShadow();

    if (!dgBangerInstance::PropShadows
        || MMSTATE->TimeOfDay == 3
        || MMSTATE->WeatherType != 0
        || lvlLevel::GetSingleton()->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
        return;

    auto timeWeather = cityLevel::GetCurrentLighting();

    bool prevLighting = gfxRenderState::SetLighting(true);

    //invert model faces
    auto prevCullMode = gfxRenderState::GetCullMode();
    gfxRenderState::SetCullMode(D3DCULL_CCW);

    auto shaders = this->GetShader(this->GetVariant());
    modStatic* model = this->GetGeomBase()->GetHighestLOD();

    if (model != nullptr)
    {
        Matrix34 shadowMatrix, dummyMatrix;
        Matrix34 bangerMatrix = this->GetMatrix(dummyMatrix);
        if (lvlInstance::ComputeShadowProjectionMatrix(shadowMatrix, this->GetRoomId(), timeWeather->KeyPitch, timeWeather->KeyHeading, bangerMatrix, this))
        {
            gfxRenderState::SetWorldMatrix(shadowMatrix);
            model->DrawShadowed(shaders, ComputeShadowIntensity(timeWeather->KeyColor));
        }
    }

    //revert model faces back
    gfxRenderState::SetCullMode(prevCullMode);
    gfxRenderState::SetLighting(prevLighting);
}

AGE_API void dgBangerInstance::DrawShadowMap()                                   { hook::Thunk<0x4419A0>::Call<void>(this); }

AGE_API void dgBangerInstance::DrawGlow()
{
    if (DefaultGlowTexture.get() == NULL)
        return;
    if ((this->GetFlags() & lvlInstance::INST_ACTIVE) == 0)
        return;

    Matrix34 dummyMatrix = Matrix34();
    Matrix34 bangerMatrix = this->GetMatrix(dummyMatrix);

    //first time texture load
    if (!GlowLoaded)
    {
        RedGlowTexture = gfxGetTexture("s_red_glow", true);
        GlowLoaded = true;
    }

    //prepare glow texture
    dgBangerData* data = this->GetData();
    gfxTexture* lastTexture = DefaultGlowTexture.get();
    bool swappedTexture = false;

    if (!strcmp(data->GetName(), "sp_light_red_f") && lastTexture != NULL)
    {
        swappedTexture = true;
        DefaultGlowTexture = RedGlowTexture;
    }

    //draw glows
    ltLight::DrawGlowBegin();
    for (int i = 0; i < data->NumGlows; i++)
    {
        tglDrawCustomParticle(bangerMatrix, &data->GlowDatas[i], DefaultGlowTexture.get(), this);
    }
    ltLight::DrawGlowEnd();

    //reset glow texture
    if (swappedTexture)
    {
        DefaultGlowTexture = lastTexture;
    }
}

AGE_API void dgBangerInstance::DrawReflected(float intensity)                    { hook::Thunk<0x4417B0>::Call<void>(this, intensity); }

AGE_API void dgBangerInstance::DrawReflectedParts(int lod)
{
    auto data = this->GetData();

    int LOD = lod;
    if ((this->GetFlags() & 4) != 0 && lod < 3)
        LOD = lod + 1;

    if (!((data->BillFlags & 512) != 0 && dgTreeRenderer::GetInstance()))
    {
        Matrix34 dummyMatrix = Matrix34();
        Matrix34 bangerMatrix = this->GetMatrix(dummyMatrix);
        gfxRenderState::SetWorldMatrix(bangerMatrix);
        gfxRenderState::SetBlendSet(0, 0x80);

        auto model = this->GetGeom(LOD, 0);
        if (model != nullptr)
        {
            if (data->BillFlags >= 0)
            {
                model->DrawGlass(this->GetGeomBase()->pShaders[this->GetVariant()]);
            }
            else
            {
                byte alphaRef = gfxRenderState::GetAlphaRef();
                gfxRenderState::SetAlphaRef((byte)sm_RefAlpha.get());
                gfxRenderState::SetTouchedMask(false);
                gfxRenderState::SetLighting(false);
                model->DrawGlass(this->GetGeomBase()->pShaders[this->GetVariant()]);
                gfxRenderState::SetTouchedMask(true);
                gfxRenderState::SetLighting(true);
                gfxRenderState::SetAlphaRef(alphaRef);
            }
        }
    }
}

AGE_API phBound* dgBangerInstance::GetBound(int type)                            { return hook::Thunk<0x442580>::Call<phBound*>(this, type); }

/* 
    dgBangerInstance virtuals
*/

AGE_API int dgBangerInstance::ComputeLod(float a1, float a2)                     { return hook::Thunk<0x4415C0>::Call<int>(this, a1, a2); }
AGE_API int dgBangerInstance::SetupGfxLights(Matrix34 const & a1) const          { return hook::Thunk<0x442910>::Call<int>(&a1); }

void dgBangerInstance::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<dgBangerInstance, lvlInstance>("dgBangerInstance")
        //functions
        .addFunction("GetData", &GetData)
        .addProperty("BangerType", &GetBangerType, &SetBangerType)
    .endClass();
}

/*
    dgUnhitBangerInstance
*/

dgUnhitBangerInstance* dgUnhitBangerInstance::requestBangerLua(LPCSTR name, bool fullMatrix)
{
    return dgUnhitBangerInstance::RequestBanger(name, fullMatrix ? TRUE : FALSE);
}

AGE_API dgUnhitBangerInstance::dgUnhitBangerInstance()
{
    scoped_vtable x(this);
    hook::Thunk<0x541880>::Call<void>(this);
}

AGE_API dgUnhitBangerInstance::~dgUnhitBangerInstance()
{
    scoped_vtable x(this);
    hook::Thunk<0x5418B0>::Call<void>(this);
}

AGE_API dgUnhitBangerInstance* dgUnhitBangerInstance::RequestBanger(const char* name, BOOL fullMatrix)
                                                                                 { return hook::StaticThunk<0x441D80>::Call<dgUnhitBangerInstance*>(name, fullMatrix); }
/*
    lvlInstance virtuals
*/

AGE_API int dgUnhitBangerInstance::Init(const char *a1, const Matrix34 &a2, int a3)
                                                                                 { return hook::Thunk<0x441C30>::Call<int>(this, a1, &a2, a3); }
AGE_API void dgUnhitBangerInstance::Reset()                                      { hook::Thunk<0x441EE0>::Call<void>(this); }

/*
    dgBangerInstance virtuals
*/

AGE_API void dgUnhitBangerInstance::Impact(lvlInstance* a1, Vector3* a2)         { hook::Thunk<0x442010>::Call<void>(this, a1, a2); };
AGE_API void dgUnhitBangerInstance::ImpactCB(dgHitBangerInstance* a1)            { hook::Thunk<0x442AD0>::Call<void>(this, a1); };

void dgUnhitBangerInstance::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<dgUnhitBangerInstance, dgBangerInstance>("dgUnhitBangerInstance")
        .addStaticFunction("RequestBanger", &requestBangerLua)
    .endClass();
}

/*
    dgHitBangerInstance
*/

AGE_API dgHitBangerInstance::dgHitBangerInstance()
{
    scoped_vtable x(this);
    hook::Thunk<0x442A60>::Call<void>(this);
}

AGE_API dgHitBangerInstance::~dgHitBangerInstance()
{
    scoped_vtable x(this);
    hook::Thunk<0x4429B0>::Call<void>(this);
}

/*
    lvlInstance virtuals
*/

AGE_API const Vector3& dgHitBangerInstance::GetPosition()                        { return hook::Thunk<0x441B70>::Call<const Vector3&>(this); }
AGE_API const Matrix34& dgHitBangerInstance::GetMatrix(Matrix34& a1)             { return hook::Thunk<0x441B60>::Call<const Matrix34&>(this, &a1); };
AGE_API void dgHitBangerInstance::SetMatrix(const Matrix34& a1)                  { hook::Thunk<0x441B40>::Call<void>(this, a1); }
AGE_API unsigned int dgHitBangerInstance::SizeOf(void)                           { return hook::Thunk<0x442AB0>::Call<unsigned int>(this); }
AGE_API void dgHitBangerInstance::Detach()                                       { return hook::Thunk<0x442680>::Call<void>(this); }

void dgHitBangerInstance::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<dgHitBangerInstance, dgBangerInstance>("dgHitBangerInstance")
    .endClass();
}

/*
    dgUnhitYBangerInstance
*/

/*
    lvlInstance virtuals
*/

AGE_API const Vector3& dgUnhitYBangerInstance::GetPosition()                     { return hook::Thunk<0x441FC0>::Call<const Vector3&>(this); }
AGE_API const Matrix34& dgUnhitYBangerInstance::GetMatrix(Matrix34& a1)          { return hook::Thunk<0x441F70>::Call<const Matrix34&>(this, &a1); };
AGE_API void dgUnhitYBangerInstance::SetMatrix(const Matrix34& a1)               { hook::Thunk<0x441F40>::Call<void>(this, &a1); }
AGE_API unsigned int dgUnhitYBangerInstance::SizeOf(void)                        { return hook::Thunk<0x442AE0>::Call<unsigned int>(this); }

void dgUnhitYBangerInstance::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<dgUnhitYBangerInstance, dgUnhitBangerInstance>("dgUnhitYBangerInstance")
        .addFactory([]() {
        return new dgUnhitYBangerInstance();
            })
    .endClass();
}

/*
    dgUnhitMtxBangerInstance
*/

/*
    lvlInstance virtuals
*/

AGE_API const Vector3& dgUnhitMtxBangerInstance::GetPosition()                   { return hook::Thunk<0x442000>::Call<const Vector3&>(this); }
AGE_API const Matrix34& dgUnhitMtxBangerInstance::GetMatrix(Matrix34& a1)        { return hook::Thunk<0x441FF0>::Call<const Matrix34&>(this, &a1); };
AGE_API void dgUnhitMtxBangerInstance::SetMatrix(const Matrix34& a1)             { hook::Thunk<0x441FD0>::Call<void>(this, a1); }
AGE_API unsigned int dgUnhitMtxBangerInstance::SizeOf(void)                      { return hook::Thunk<0x442AF0>::Call<unsigned int>(this); }

void dgUnhitMtxBangerInstance::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<dgUnhitMtxBangerInstance, dgUnhitBangerInstance>("dgUnhitMtxBangerInstance")
        .addFactory([]() {
        return new dgUnhitMtxBangerInstance();
            })
    .endClass();
}

/* 
    Manager
*/

dgBangerManager* dgBangerManager::GetInstance()
{
    return Instance.get();
}

/*
    asNode virtuals
*/

AGE_API void dgBangerManager::Reset()                                            { hook::Thunk<0x4428A0>::Call<void>(this); }

/*
    dgBangerManager
*/

AGE_API dgHitBangerInstance * dgBangerManager::GetBanger()                       { return hook::Thunk<0x442780>::Call<dgHitBangerInstance*>(this); }
AGE_API void dgBangerManager::Init(int poolSize)                                 { hook::Thunk<0x4427E0>::Call<void>(this, poolSize); }

void dgBangerManager::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<dgBangerManager, asNode>("dgBangerManager")
        .addStaticProperty("Instance", &dgBangerManager::GetInstance)
        .addFunction("GetBanger", &GetBanger)
    .endClass();
}