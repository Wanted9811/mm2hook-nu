#pragma once
#include "aiCableCar.h"

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
AGE_API void aiCableCarInstance::Draw(int lod)                                        { hook::Thunk<0x541680>::Call<void>(this, lod); }
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