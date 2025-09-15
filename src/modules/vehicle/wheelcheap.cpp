#pragma once
#include "wheelcheap.h"

using namespace MM2;

Vector3 vehWheelCheap::GetPosition() const
{
    return this->Position;
}

Matrix34 vehWheelCheap::GetMatrix() const
{
    return this->Matrix;
}

Matrix34 vehWheelCheap::GetWorldMatrix() const
{
    return this->WorldMatrix;
}

AGE_API vehWheelCheap::vehWheelCheap()
{
    hook::Thunk<0x59D790>::Call<void>(this);
}

AGE_API vehWheelCheap::~vehWheelCheap()
{
    hook::Thunk<0x554360>::Call<void>(this);
}

AGE_API void vehWheelCheap::Init(Vector3* wheelPosition, aiVehicleData* data, phInertialCS* ICS)
{
    hook::Thunk<0x59D890>::Call<void>(this, wheelPosition, data, ICS);
}

AGE_API void vehWheelCheap::Update()
{
    hook::Thunk<0x59D970>::Call<void>(this);
}

AGE_API void vehWheelCheap::Reset()
{
    hook::Thunk<0x59D860>::Call<void>(this);
}

void vehWheelCheap::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<vehWheelCheap, asNode>("vehWheelCheap")
        //properties
        .addPropertyReadOnly("Position", &GetPosition)
        .addPropertyReadOnly("Matrix", &GetMatrix)
        .addPropertyReadOnly("WorldMatrix", &GetWorldMatrix)
    .endClass();
}