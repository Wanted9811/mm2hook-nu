#pragma once
#include "aiCableCar.h"
#include "aiMap.h"
#include <modules\city\citylevel.h>
#include <modules\mmcityinfo\state.h>

using namespace MM2;

/*
    aiCableCar
*/

aiCableCarInstance* aiCableCar::GetInstance()
{
    return this->Instance;
}

/*
    aiObstacle virtuals
*/

AGE_API aiCableCar::aiCableCar()                                                      { hook::Thunk<0x53F700>::Call<void>(this); }
AGE_API aiCableCar::~aiCableCar()                                                     { hook::Thunk<0x53F780>::Call<void>(this); }
AGE_API void aiCableCar::Position(Vector3 &position)                                  { hook::Thunk<0x541840>::Call<void>(this, &position); }
AGE_API float aiCableCar::Speed()                                                     { return hook::Thunk<0x5417E0>::Call<float>(this); }
AGE_API int aiCableCar::CurrentRoadIdx(aiPath** a1, const bool* a2, int* a3)          { return hook::Thunk<0x5410E0>::Call<int>(this, a1, a2, a3); }
AGE_API int aiCableCar::CurrentRdVert()                                               { return hook::Thunk<0x5411F0>::Call<int>(this); }

/*
    aiVehicle virtuals
*/

AGE_API void aiCableCar::Update()                                                     { hook::Thunk<0x53FBD0>::Call<void>(this); }
AGE_API void aiCableCar::Reset()                                                      { hook::Thunk<0x53F990>::Call<void>(this); }
AGE_API int aiCableCar::Type()                                                        { return hook::Thunk<0x5417D0>::Call<int>(this); }
AGE_API Matrix34& aiCableCar::GetMatrix() const                                       { return hook::Thunk<0x541860>::Call<Matrix34&>(this); }
AGE_API float aiCableCar::FrontBumperDistance()                                       { return hook::Thunk<0x541800>::Call<float>(this); }
AGE_API float aiCableCar::BackBumperDistance()                                        { return hook::Thunk<0x541810>::Call<float>(this); }
AGE_API float aiCableCar::LSideDistance()                                             { return hook::Thunk<0x541820>::Call<float>(this); }
AGE_API float aiCableCar::RSideDistance()                                             { return hook::Thunk<0x541830>::Call<float>(this); }
AGE_API int aiCableCar::CurrentLane()                                                 { return hook::Thunk<0x5417F0>::Call<int>(this); }
AGE_API int aiCableCar::CurrentRoadId()                                               { return hook::Thunk<0x541870>::Call<int>(this); }

AGE_API void aiCableCar::DrawId() const
{
    Vector3 position = GetMatrix().GetRow(3);
    position.Y += 4.4f;
    vglDrawLabelf(position, "%d", this->GetId());
}

void aiCableCar::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<aiCableCar, aiVehicle>("aiCableCar")
        .addPropertyReadOnly("Instance", &GetInstance)
    .endClass();
}

/*
    aiCableCarInstance
*/

aiCableCar* aiCableCarInstance::GetCableCar()
{
    return this->CableCar;
}

/*
    lvlInstance virtuals
*/

AGE_API Vector3 const & aiCableCarInstance::GetPosition()                             { return hook::Thunk<0x541650>::Call<Vector3 const &>(this); }
AGE_API Matrix34 const & aiCableCarInstance::GetMatrix(Matrix34 &matrix)              { return hook::Thunk<0x5415B0>::Call<Matrix34 const &>(this, &matrix); }
AGE_API void aiCableCarInstance::SetMatrix(const Matrix34 &matrix)                    { hook::Thunk<0x5415C0>::Call<void>(this, &matrix); }
AGE_API Vector3 const & aiCableCarInstance::GetVelocity()                             { return hook::Thunk<0x5415E0>::Call<Vector3 const &>(this); }

AGE_API void aiCableCarInstance::Draw(int lod)
{
    if (lod < 0 || lod > 3)
        return;
    if (this->GetGeomIndex() == 0)
        return;

    //get shaders
    auto shaders = this->GetShader(0);

    //setup renderer
    gfxRenderState::SetWorldMatrix(this->GetCableCar()->GetMatrix());

    //get cable car
    modStatic* cableCar = this->GetGeom(lod, 0);
    if (cableCar != nullptr)
        cableCar->DrawNoGlass(shaders);
}

AGE_API void aiCableCarInstance::DrawShadow()
{
    if (this->GetGeomIndex() == 0)
        return;

    //get shaders
    auto shaders = this->GetShader(0);

    //get shadow matrix
    Matrix34 shadowMatrix = Matrix34();
    Matrix34 cableCarMatrix = this->GetCableCar()->GetMatrix();

    if (!vehCarModel::Enable3DShadows
        || MMSTATE->TimeOfDay == 3
        || lvlLevel::GetSingleton()->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
    {
        if (lvlInstance::ComputeShadowMatrix(shadowMatrix, this->GetRoomId(), cableCarMatrix))
        {
            //setup renderer
            gfxRenderState::SetWorldMatrix(shadowMatrix);

            //draw drop shadow
            modStatic* shadow = this->GetGeomBase(SHADOW_GEOM_ID)->GetHighestLOD();
            if (shadow != nullptr)
                shadow->Draw(shaders);
        }
    }
    else
    {
        auto timeWeather = cityLevel::GetCurrentLighting();

        bool prevLighting = gfxRenderState::SetLighting(true);

        //invert model faces
        auto prevCullMode = gfxRenderState::GetCullMode();
        gfxRenderState::SetCullMode(D3DCULL_CCW);

        //get cable car
        modStatic* cableCar = this->GetGeomBase(0)->GetHighestLOD();
        if (cableCar != nullptr)
        {
            if (lvlInstance::ComputeShadowProjectionMatrix(shadowMatrix, this->GetRoomId(), timeWeather->KeyPitch, timeWeather->KeyHeading, cableCarMatrix, this))
            {
                //setup renderer
                gfxRenderState::SetWorldMatrix(shadowMatrix);

                //draw 3D shadow
                cableCar->DrawShadowed(shaders, ComputeShadowIntensity(timeWeather->KeyColor));
            }
        }

        //revert model faces back
        gfxRenderState::SetCullMode(prevCullMode);
        gfxRenderState::SetLighting(prevLighting);
    }
}

AGE_API void aiCableCarInstance::DrawGlow()
{
    //We only draw a headlight glow here. Bail immediately if we can
    if (!aiMap::GetInstance()->showHeadlights)
        return;
    if (this->GetGeomIndex() == 0)
        return;

    //get data
    auto data = this->GetData();

    //get shaders
    auto shaders = this->GetShader(0);

    //setup renderer
    Matrix34 cableCarMatrix = this->GetCableCar()->GetMatrix();
    gfxRenderState::SetWorldMatrix(cableCarMatrix);

    //draw hlight
    modStatic* hlight = this->GetGeomBase(HLIGHT_GEOM_ID)->GetHighestLOD();
    if (hlight != nullptr)
        hlight->Draw(shaders);

    //draw light glow
    ltLight::DrawGlowBegin();
    for (int i = 0; i < data->NumGlows; i++)
    {
        tglDrawCustomParticle(cableCarMatrix, &data->GlowDatas[i], ltLight::GlowTexture.get(), this);
    }
    ltLight::DrawGlowEnd();
}

AGE_API void aiCableCarInstance::DrawReflected(float intensity)
{ 
    if (this->GetGeomIndex() == 0)
        return;

    //get shaders
    auto shaders = this->GetShader(0);

    //setup renderer
    Matrix34 cableCarMatrix = this->GetCableCar()->GetMatrix();
    gfxRenderState::SetWorldMatrix(cableCarMatrix);

    //get cable car
    modStatic* cableCar = this->GetGeom(3, 0);
    if (cableCar != nullptr)
        cableCar->DrawReflected(shaders, this->GetRoomId(), cableCarMatrix, intensity);
}

AGE_API void aiCableCarInstance::DrawReflectedParts(int lod)
{ 
    if (lod < 0 || lod > 3)
        return;
    if (this->GetGeomIndex() == 0)
        return;

    //get shaders
    auto shaders = this->GetShader(0);

    //setup renderer
    gfxRenderState::SetWorldMatrix(this->GetCableCar()->GetMatrix());

    //get cable car
    modStatic* cableCar = this->GetGeom(lod, 0);
    if (cableCar != nullptr)
        cableCar->DrawGlass(shaders);
}

AGE_API unsigned int aiCableCarInstance::SizeOf()                                     { return hook::Thunk<0x541660>::Call<unsigned int>(this); }

/*
    dgBangerInstance virtuals
*/

AGE_API void aiCableCarInstance::ImpactCB(dgHitBangerInstance* hitBangInst)           { hook::Thunk<0x541670>::Call<void>(this, hitBangInst); }

void aiCableCarInstance::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<aiCableCarInstance, dgUnhitBangerInstance>("aiCableCarInstance")
        .addPropertyReadOnly("CableCar", &GetCableCar)
        .addFunction("Position", &GetPosition)
        .addFunction("Velocity", &GetVelocity)
        .addFunction("Matrix", &GetMatrix)
    .endClass();
}