#pragma once
#include "aiVehicleAmbient.h"
#include <modules\rgl.h>

using namespace MM2;

AGE_API aiVehicleAmbient::aiVehicleAmbient()                         { hook::Thunk<0x551340>::Call<void>(this); }
AGE_API aiVehicleAmbient::~aiVehicleAmbient()                        { hook::Thunk<0x551370>::Call<void>(this); }

AGE_API void aiVehicleAmbient::Update()                              { hook::Thunk<0x551600>::Call<void>(this); }
AGE_API void aiVehicleAmbient::Reset()                               { hook::Thunk<0x551520>::Call<void>(this); }
AGE_API int aiVehicleAmbient::Type()                                 { return hook::Thunk<0x551CC0>::Call<int>(this); }
AGE_API void aiVehicleAmbient::DrawId() const                        { aiVehicleSpline::DrawId(); }
AGE_API void aiVehicleAmbient::ReplayDebug()                         { hook::Thunk<0x551950>::Call<void>(this); }
AGE_API void aiVehicleAmbient::Impact(int a1)                        { hook::Thunk<0x5519C0>::Call<void>(this, a1); }

//members
AGE_API void aiVehicleAmbient::Init(const char* name, int id)        { hook::Thunk<0x551380>::Call<void>(this, name, id); }
AGE_API void aiVehicleAmbient::Dump()                                { hook::Thunk<0x551A80>::Call<void>(this); }

void aiVehicleAmbient::DrawBBox(bool selected)
{
    auto matrix = this->GetMatrix();
    Vector3 frontleft = Vector3(-LSideDistance(), 1.0f, -FrontBumperDistance());
    Vector3 frontright = Vector3(RSideDistance(), 1.0f, -FrontBumperDistance());
    Vector3 backleft = Vector3(-LSideDistance(), 1.0f, BackBumperDistance());
    Vector3 backright = Vector3(RSideDistance(), 1.0f, BackBumperDistance());

    if (selected)
        vglCurrentColor = mkfrgba(0.0f, 0.0f, 1.0f, 1.0f);
    else
        vglCurrentColor = mkfrgba(1.0f, 0.0f, 0.0f, 1.0f);

    vglBegin(gfxDrawMode::DRAWMODE_LINESTRIP, 5);
    vglVertex3f(matrix.Transform(frontleft));
    vglVertex3f(matrix.Transform(frontright));
    vglVertex3f(matrix.Transform(backright));
    vglVertex3f(matrix.Transform(backleft));
    vglVertex3f(matrix.Transform(frontleft));
    vglEnd();
}

//lua
void aiVehicleAmbient::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<aiVehicleAmbient, aiVehicleSpline>("aiVehicleAmbient")
        .addFunction("Dump", &Dump)
        .addFunction("DrawBBox", &DrawBBox)
    .endClass();
}