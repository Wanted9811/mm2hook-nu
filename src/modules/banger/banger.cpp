#pragma once
#include "banger.h"

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

/*
    lvlInstance virtuals
*/

AGE_API void dgBangerInstance::SetVariant(int a1)                                { hook::Thunk<0x442A10>::Call<void>(this); }
AGE_API dgPhysEntity* dgBangerInstance::GetEntity()                              { return hook::Thunk<0x441AD0>::Call<dgPhysEntity*>(this); }
AGE_API dgPhysEntity* dgBangerInstance::AttachEntity()                           { return hook::Thunk<0x441AE0>::Call<dgPhysEntity*>(this); }
AGE_API Vector3 const & dgBangerInstance::GetVelocity()                          { return hook::Thunk<0x441B20>::Call<Vector3 const &>(this); }
AGE_API void dgBangerInstance::Draw(int a1)                                      { hook::Thunk<0x4415E0>::Call<void>(this, a1); }
AGE_API void dgBangerInstance::DrawShadow()                                      { hook::Thunk<0x441990>::Call<void>(this); }
AGE_API void dgBangerInstance::DrawShadowMap()                                   { hook::Thunk<0x4419A0>::Call<void>(this); }
AGE_API void dgBangerInstance::DrawGlow()                                        { hook::Thunk<0x441840>::Call<void>(this); }
AGE_API void dgBangerInstance::DrawReflected(float a1)                           { hook::Thunk<0x4417B0>::Call<void>(this, a1); }
AGE_API phBound* dgBangerInstance::GetBound(int a1)                              { return hook::Thunk<0x442580>::Call<phBound*>(this, a1); }

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
AGE_API const Matrix34& dgHitBangerInstance::GetMatrix(Matrix34* a1)             { return hook::Thunk<0x441B60>::Call<const Matrix34&>(this, a1); };
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