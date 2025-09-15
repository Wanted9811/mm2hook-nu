#pragma once
#include "citylevel.h"

namespace MM2
{
    declfield(cityLevel::Sky)(0x628728);
    declfield(cityLevel::sm_EnablePVS)(0x62B070);
    declfield(cityLevel::sm_RefAlpha)(0x5C5718);
    declfield(cityLevel::sm_ShadowHFudge)(0x5C5714);
    declfield(cityLevel::sm_Lighting)(0x629998);
    declfield(cityLevel::sm_PvsBuffer)(0x62AE68);
    declfield(cityLevel::SDL)(0x629928);
    declfield(cityLevel::RainParticles)(0x62770C);
    declfield(cityLevel::RainBirthRule)(0x629990);

    hook::Type<int> timeOfDay(0x62B068);
    hook::Type< cityTimeWeatherLighting[16]> timeWeathers(0x6299A8);

    bool cityLevel::EnableMovingClouds = true;

    /*
        cityTimeWeatherLighting
    */

    void cityTimeWeatherLighting::FileIO(datParser& parser)
    {
        hook::Thunk<0x443440>::Call<void>(this, &parser);
    }

    void cityTimeWeatherLighting::SetAmbient(Vector4 ambient)
    {
        ambient.W = 1.0f;
        Ambient = ambient.PackColorBGRA();
    }

    Vector4 cityTimeWeatherLighting::GetAmbient() const
    {
        Vector4 vec;
        vec.UnpackColorBGRA(Ambient);
        vec.W = 1.0f;
        return vec;
    }

    void cityTimeWeatherLighting::ComputeAmbientLightLevels()
    {
        hook::Thunk<0x443300>::Call<void>(this);
    }

    void cityTimeWeatherLighting::BindLua(LuaState L)
    {
        LuaBinding(L).beginClass<cityTimeWeatherLighting>("cityTimeWeatherLighting")
            .addVariable("KeyHeading", &cityTimeWeatherLighting::KeyHeading)
            .addVariable("KeyPitch", &cityTimeWeatherLighting::KeyPitch)
            .addVariable("KeyColor", &cityTimeWeatherLighting::KeyColor)

            .addVariable("Fill1Heading", &cityTimeWeatherLighting::Fill1Heading)
            .addVariable("Fill1Pitch", &cityTimeWeatherLighting::Fill1Pitch)
            .addVariable("Fill1Color", &cityTimeWeatherLighting::Fill1Color)

            .addVariable("Fill2Heading", &cityTimeWeatherLighting::Fill2Heading)
            .addVariable("Fill2Pitch", &cityTimeWeatherLighting::Fill2Pitch)
            .addVariable("Fill2Color", &cityTimeWeatherLighting::Fill2Color)

            .addProperty("Ambient", &GetAmbient, &SetAmbient)
            
            .addFunction("ComputeAmbientLightLevels", &ComputeAmbientLightLevels)
            .addFunction("FileIO", &FileIO)
            .endClass();
    }

    void SetLightDirection(Vector3* out, float heading, float pitch)
    {
        out->X = cosf(heading) * cosf(pitch);
        out->Y = sinf(pitch);
        out->Z = sinf(heading) * cosf(pitch);
    }

    float ComputeShadowIntensity(Vector3 keyColor)
    {
        float average = (keyColor.X + keyColor.Y + keyColor.Z) / 3.0f;
        return min(average, 0.6f);
    }

    /*
        cityLevel
    */

    std::vector<Vector3> cityLevel::LuaGetPerimeter(int room)
    {
        auto vec = std::vector<Vector3>();
        vec.resize(64);
        int actual_size = this->GetRoomPerimeter(room, vec.data(), vec.size());
        vec.resize(actual_size);
        return vec;
    }

    AGE_API cityLevel::cityLevel() {
        scoped_vtable x(this);
        hook::Thunk<0x443860>::Call<void>(this);
    };

    AGE_API cityLevel::~cityLevel() {
        scoped_vtable x(this);
        hook::Thunk<0x443910>::Call<void>(this);
    };

    /*
        lvlLevel virtuals
    */

    AGE_API void cityLevel::Load(char const* a1)          { hook::Thunk<0x443F30>::Call<void>(this, a1); }
    AGE_API void cityLevel::Update()                      { hook::Thunk<0x4452B0>::Call<void>(this); }
    AGE_API void cityLevel::PreDraw()                     { hook::Thunk<0x4452E0>::Call<void>(this); }
    AGE_API void cityLevel::PostDraw()                    { hook::Thunk<0x445300>::Call<void>(this); }
    AGE_API void cityLevel::Draw(const gfxViewport& a1, uint a2) 
                                                                { hook::Thunk<0x445400>::Call<void>(this, &a1, a2); }

    AGE_API int cityLevel::FindRoomId(Vector3 const& position, int previousRoom) const
                                                        { return hook::Thunk<0x446A60>::Call<int>(this, &position, previousRoom); }
    AGE_API int cityLevel::GetNeighborCount(int room) const 
                                                        { return hook::Thunk<0x446C20>::Call<int>(this, room); }
    AGE_API int cityLevel::GetNeighbors(int* neighbourRooms, int room) const  
                                                        { return hook::Thunk<0x446C40>::Call<int>(this, neighbourRooms, room); }
    AGE_API int cityLevel::GetTouchedNeighbors(int* a1, int a2, int a3, const Vector4& a4)
                                                        { return hook::Thunk<0x446CD0>::Call<int>(this, a1, a2, a3, a4); }
    AGE_API int cityLevel::GetRoomPerimeter(int roomId, Vector3* out, int outSize) const
                                                        { return hook::Thunk<0x446FA0>::Call<int>(this, roomId, out, outSize); }
    AGE_API int cityLevel::GetVisitList(int* a1, int a2, Vector3 const& a3, Vector3 const& a4, int a5, int a6)
                                                        { return hook::Thunk<0x447030>::Call<int>(this, a1, a2, a3, a4, a5, a6); }
    AGE_API bool cityLevel::Collide(int a1, lvlSegment& a2, lvlIntersection& a3)
                                                        { return false; }
    AGE_API bool cityLevel::GetBoundSphere(Vector4& a1, int a2)
                                                        { return hook::Thunk<0x445310>::Call<bool>(this, a1, a2); }
    AGE_API const class lvlLevelBound* cityLevel::GetBound()
                                                        { return hook::Thunk<0x443930>::Call<class lvlLevelBound*>(this); }
    AGE_API void cityLevel::SetObjectDetail(int a1)       { hook::Thunk<0x443E50>::Call<void>(this, a1); }
    AGE_API float cityLevel::GetWaterLevel(int room) const  { return hook::Thunk<0x445280>::Call<float>(this, room); }
    AGE_API float cityLevel::GetLightingIntensity(Vector3 const& a1) const
                                                        { return hook::Thunk<0x445290>::Call<float>(this, a1); }
    AGE_API void cityLevel::SetPtxHeight(asParticles& a1) { hook::Thunk<0x4452A0>::Call<void>(this, &a1); }

    AGE_API gfxTexture* cityLevel::GetEnvMap(int room, Vector3 const& position, float& intensity)
                                                                { return hook::Thunk<0x443940>::Call<gfxTexture*>(this, room, &position, &intensity); }


    /*
        cityLevel functions
    */
    AGE_API void cityLevel::EnablePVS(bool enable) {
        hook::Thunk<0x4472A0>::Call<void>(this, enable);
    }

    AGE_API void cityLevel::EnableSky(bool enable) {
        hook::Thunk<0x447290>::Call<void>(this, enable);
    }

    AGE_API void cityLevel::LoadPath(const char* a1, const Matrix34& a2, bool a3) {
        hook::StaticThunk<0x4451E0>::Call<void>(a1, a2, a3);
    }

    AGE_API void cityLevel::LoadPathSet(const char* a1, const char* a2) {
        hook::StaticThunk<0x445140>::Call<void>(a1, a2);
    }

    AGE_API void cityLevel::LoadProp(int a1, const char* a2, const Matrix34& a3) {
        hook::StaticThunk<0x4450E0>::Call<void>(a1, a2, a3);
    }

    //should be protected but we use this in the hook
    AGE_API void cityLevel::DrawRooms(const gfxViewport& a1, uint a2, const cityRoomRec* a3, int a4) {
        hook::Thunk<0x445820>::Call<void>(this, &a1, a2, a3, a4);
    }

    AGE_API void cityLevel::SetupLighting(Vector3 const& multiplyColor)
    {
        hook::StaticThunk<0x4436A0>::Call<void>(&multiplyColor);
    }

    AGE_API void cityLevel::SetupPerRoomLighting(int roomId)
    {
        hook::Thunk<0x4457B0>::Call<void>(this, roomId);
    }

    /*
        Helpers
    */
    bool cityLevel::IsRoomVisible(int roomId) const
    {
        if (!sm_EnablePVS.get())
            return true;
        if (roomId < 0 || roomId >= this->GetRoomCount())
            return false;

        auto buf = sm_PvsBuffer.ptr();
        return  ((buf[roomId >> 2] >> (2 * (roomId & 3))) & 3) != 0;
    }

    lvlSDL* cityLevel::GetSDL()
    {
        return SDL.ptr();
    }

    int cityLevel::GetRefAlpha()
    {
        return sm_RefAlpha.get();
    }

    float cityLevel::GetShadowHFudge()
    {
        return sm_ShadowHFudge.get();
    }

    Vector3 cityLevel::GetSmLighting()
    {
        return sm_Lighting.get();
    }

    asParticles* cityLevel::GetRainParticles()
    {
        return RainParticles.get();
    }

    asBirthRule* cityLevel::GetRainBirthRule()
    {
        return RainBirthRule.get();
    }

    cityTimeWeatherLighting* cityLevel::GetLighting(int index)
    {
        return timeWeathers.ptr(index);
    }

    cityTimeWeatherLighting* cityLevel::GetCurrentLighting()
    {
        return timeWeathers.ptr(timeOfDay.get());
    }

    //lua
    void cityLevel::BindLua(LuaState L) {
        LuaBinding(L).beginExtendClass<cityLevel, lvlLevel>("cityLevel")
            //functions
            .addFunction("GetRoomPerimeter", &LuaGetPerimeter)

            .addFunction("EnablePVS", &EnablePVS)
            .addFunction("EnableSky", &EnableSky)
            .addFunction("IsRoomVisible", &IsRoomVisible)

            .addStaticFunction("LoadPath", &LoadPath)
            .addStaticFunction("LoadPathSet", &LoadPathSet)
            .addStaticFunction("LoadProp", &LoadProp)

            .addStaticProperty("SDL", &GetSDL)
            .addStaticFunction("GetLighting", &GetLighting)
            .addStaticFunction("GetCurrentLighting", &GetCurrentLighting)
                
            //singleton
            .addStaticProperty("Singleton", [] 
            { 
                //MM2 only uses cityLevel so this works
                auto level = lvlLevel::GetSingleton();
                return reinterpret_cast<cityLevel*>(level);
            })

            //sky singleton
            .addStaticProperty("Sky", []() -> lvlSky* { return &Sky; })
            .endClass();
    }
}