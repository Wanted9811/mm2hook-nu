#include "camera.h"
#include <modules\vehicle.h>

using namespace MM2;

/*
    asCamera
*/

AGE_API asCamera::asCamera(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x4A2340>::Call<void>(this);
};

AGE_API asCamera::~asCamera(void) {
    scoped_vtable x(this);
    hook::Thunk<0x4A22E0>::Call<void>(this);
};

AGE_API void asCamera::SetView(float fov, float aspect, float nearClip, float farClip)
{
    hook::Thunk<0x4A2E50>::Call<void>(this, fov, aspect, nearClip, farClip);
}

AGE_API void asCamera::SetViewport(float left, float top, float width, float height)
{
    hook::Thunk<0x4A2DD0>::Call<void>(this, left, top, width, height, 1); // last arg appears unused
}

AGE_API void asCamera::Update()
{
    hook::Thunk<0x4A24A0>::Call<void>(this);
}

//lua
void asCamera::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<asCamera, asNode>("asCamera")
        .addFunction("SetView", &SetView)
        .addFunction("SetViewport", &SetViewport)
        .endClass();
}

/*
    camBaseCS
*/

AGE_API camBaseCS::camBaseCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x521D60>::Call<void>(this);
}

AGE_API camBaseCS::~camBaseCS(void)
{

}

//fields
Matrix34 camBaseCS::GetMatrix() const
{
    return this->WorldMatrix;
}

void camBaseCS::SetMatrix(Matrix34 matrix)
{
    this->WorldMatrix = matrix;
}
        
Vector3 camBaseCS::GetPosition() const
{
    return this->WorldMatrix.GetRow(3);
}

void camBaseCS::SetPosition(Vector3 position)
{
    this->WorldMatrix.SetRow(3, position);
}

float camBaseCS::GetFOV() const
{
    return this->CameraFOV;
}
        
void camBaseCS::SetFOV(float fov)
{
    this->CameraFOV = fov;
}

void camBaseCS::SetCamView(camViewCS* view)
{
    this->View = view;
}

float camBaseCS::GetCameraFar()
{
    return sm_CameraFar.get();
}

void camBaseCS::SetCameraFar(float value)
{
    sm_CameraFar.set(value);
}

//asNode overrides
AGE_API void camBaseCS::AfterLoad()                            { hook::Thunk<0x521F30>::Call<void>(this); }
AGE_API void camBaseCS::FileIO(datParser &parser)              { hook::Thunk<0x521EA0>::Call<void>(this, &parser); }
AGE_API const char * camBaseCS::GetDirName()                   { return hook::Thunk<0x521E00>::Call<const char*>(this); }

//virtuals
AGE_API void camBaseCS::MakeActive()                           { hook::Thunk<0x521520>::Call<void>(this); }
AGE_API void camBaseCS::UpdateInput()                          { hook::Thunk<0x520410>::Call<void>(this); }
AGE_API void camBaseCS::ForceMatrixDelta(const Vector3 &a1)    { hook::Thunk<0x521E60>::Call<void>(this, &a1); }
AGE_API void camBaseCS::SetST(float *a1)                       { hook::Thunk<0x51D750>::Call<void>(this, a1); }

//members
AGE_API BOOL camBaseCS::IsViewCSInTransition()                 { return hook::Thunk<0x521E10>::Call<BOOL>(this);}
AGE_API void camBaseCS::UpdateView()                           { hook::Thunk<0x521E30>::Call<void>(this); }

//lua
void camBaseCS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camBaseCS, asNode>("camBaseCS")
        //properties
        .addFactory([]() {
        auto obj = new camBaseCS();
        MM2Lua::MarkForCleanupOnShutdown(obj);
        return obj;
            })
        .addStaticProperty("CameraFar", &GetCameraFar, &SetCameraFar)

        .addProperty("FOV", &GetFOV, &SetFOV)
        .addProperty("CameraFOV", &GetFOV, &SetFOV)

        .addFunction("GetPosition", &GetPosition)
        .addFunction("GetMatrix", &GetMatrix)
        .addFunction("SetMatrix", &SetMatrix)
        .addFunction("SetPosition", &SetPosition)
        .addFunction("SetCamView", &SetCamView)

        .addVariable("BlendTime", &camBaseCS::BlendTime)
        .addVariable("BlendGoal", &camBaseCS::BlendGoal)
        .addVariable("CameraNear", &camBaseCS::CameraNear)

        //members
        .addFunction("UpdateView", &UpdateView)
    .endClass();
}

/*
    camAppCS
*/

AGE_API void camAppCS::ApproachIt()                           { hook::Thunk<0x522060>::Call<void>(this); }

AGE_API camAppCS::camAppCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x521F70>::Call<void>(this);
}

AGE_API camAppCS::~camAppCS(void)
{

}

void camAppCS::SetApproachOn(bool value) { ApproachOn = (value) ? TRUE : FALSE; }
bool camAppCS::GetApproachOn() const     { return ApproachOn == TRUE; }
void camAppCS::SetAppAppOn(bool value)   { AppAppOn = (value) ? TRUE : FALSE; }
bool camAppCS::GetAppAppOn() const       { return AppAppOn == TRUE; }

Matrix34* camAppCS::GetTarget()          { return this->Target; }

//overrides
AGE_API void camAppCS::FileIO(datParser &parser)              { hook::Thunk<0x5229D0>::Call<void>(this, &parser); }
    
//lua
void camAppCS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camAppCS, camBaseCS>("camAppCS")
        .addFactory([]() {
        auto obj = new camAppCS();
        MM2Lua::MarkForCleanupOnShutdown(obj);
        return obj;
            })
        .addVariableRef("Target", &camAppCS::Target)
        .addProperty("ApproachOn", &GetApproachOn, &SetApproachOn)
        .addProperty("AppAppOn", &GetAppAppOn, &SetAppAppOn)
        .addVariable("AppRot", &camAppCS::AppRot)
        .addVariable("AppXRot", &camAppCS::AppXRot)
        .addVariable("AppYPos", &camAppCS::AppYPos)
        .addVariable("AppXZPos", &camAppCS::AppXZPos)
        .addVariable("AppApp", &camAppCS::AppApp)
        .addVariable("AppRotMin", &camAppCS::AppRotMin)
        .addVariable("AppPosMin", &camAppCS::AppPosMin)
        .addVariable("LookAbove", &camAppCS::LookAbove)
        .addVariable("TrackTo", &camAppCS::TrackTo)
        .addVariable("MaxDist", &camAppCS::MaxDist)
        .addVariable("MinDist", &camAppCS::MinDist)
        .addVariable("LookAt", &camAppCS::LookAt)
    .endClass();
}

/*
    camCarCS
*/

vehCar* camCarCS::GetCar() const
{
    return this->Car;
}

void camCarCS::SetCar(vehCar* car)
{
    this->Car = car;
}

int camCarCS::GetReverseMode()
{
    return this->ReverseMode;
}

AGE_API camCarCS::camCarCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x521470>::Call<void>(this);
}

AGE_API camCarCS::~camCarCS(void)
{

}

AGE_API void camCarCS::Init(vehCar *car, LPCSTR name)        { hook::Thunk<0x5214A0>::Call<void>(this, car, name); }

//overrides
AGE_API void camCarCS::FileIO(datParser &parser)             { hook::Thunk<0x5214E0>::Call<void>(this, &parser); }

//lua
void camCarCS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camCarCS, camAppCS>("camCarCS")
        .addProperty("Car", &GetCar, &SetCar)
        .addVariable("ReverseMode", &camCarCS::ReverseMode)
        .addFunction("Init", &Init)
    .endClass();
}

/*
    camPointCS
*/

AGE_API camPointCS::camPointCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x520610>::Call<void>(this);
}

AGE_API camPointCS::~camPointCS(void) {
    scoped_vtable x(this);
    hook::Thunk<0x406820>::Call<void>(this);
}

float camPointCS::GetMinDist() const
{
    return this->MinDist;
}

float camPointCS::GetMaxDist() const
{
    return this->MaxDist;
}

float camPointCS::GetAppRate() const
{
    return this->AppRate;
}

AGE_API void camPointCS::SetAppRate(float rate)                 { hook::Thunk<0x520730>::Call<void>(this, rate); }
AGE_API void camPointCS::SetMaxDist(float dist)                 { hook::Thunk<0x5206F0>::Call<void>(this, dist); }
AGE_API void camPointCS::SetMinDist(float dist)                 { hook::Thunk<0x520710>::Call<void>(this, dist); }
AGE_API void camPointCS::SetPos(Vector3 const &pos)             { hook::Thunk<0x5206B0>::Call<void>(this, &pos); }
AGE_API void camPointCS::SetVel(Vector3 const &vel)             { hook::Thunk<0x520740>::Call<void>(this, &vel); }

//camBaseCS overrides
AGE_API void camPointCS::MakeActive()                           { hook::Thunk<0x5206A0>::Call<void>(this); }

//asNode overrides
AGE_API void camPointCS::Reset()                                { hook::Thunk<0x520690>::Call<void>(this); }
AGE_API void camPointCS::Update()                               { hook::Thunk<0x520790>::Call<void>(this); }
AGE_API char * camPointCS::GetClassName()                       { return hook::Thunk<0x520A20>::Call<char*>(this); }

//lua
void camPointCS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camPointCS, camCarCS>("camPointCS")
        .addFactory([]() {
        auto obj = new camPointCS();
        MM2Lua::MarkForCleanupOnShutdown(obj);
        return obj;
            })
        //properties
        .addProperty("MinDist", &GetMinDist, &SetMinDist)
        .addProperty("MaxDist", &GetMaxDist, &SetMaxDist)
        .addProperty("AppRate", &GetAppRate, &SetAppRate)
        .addFunction("SetPosition", &SetPos)
        .addFunction("SetVelocity", &SetVel)
    .endClass();
}

/*
    camPovCS
*/

AGE_API void camPovCS::UpdatePOV()
{
    float pitch = Pitch;
    float angle = Angle;

    Matrix.Identity();

    if (ReverseMode != -1)
        ReverseOffset = Offset;

    Matrix.SetRow(3, ReverseOffset);
    Matrix.Dot(*Target);

    if (ReverseMode == -1)
        pitch = -Pitch;

    if (pitch != 0.0f)
        Matrix.Rotate(Matrix.GetRow(0), pitch);

    if (ReverseMode == -1)
        angle = 3.1415927f;

    if (Yaw != 0.0f)
        angle = Yaw;

    if (angle != 0.0f)
        Matrix.Rotate(Matrix.GetRow(1), angle);

    // setup pov cam wobble
    Matrix.Set(Car->GetModel()->GetBodyWobbleMatrix(Matrix, Car->GetCarSim()->GetWheel(1), 0.005f));

    this->ApproachIt();
}

Vector3 camPovCS::GetOffset() const
{
    return this->Offset;
}

Vector3 camPovCS::GetReverseOffset() const
{
    return this->ReverseOffset;
}

float camPovCS::GetAngle() const
{
    return this->Angle;
}

float camPovCS::GetPitch() const
{
    return this->Pitch;
}

float camPovCS::GetYaw() const
{
    return this->Yaw;
}

AGE_API camPovCS::camPovCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x51D410>::Call<void>(this);
}

AGE_API camPovCS::~camPovCS(void) {
    scoped_vtable x(this);
    hook::Thunk<0x51D520>::Call<void>(this);
}

//camBaseCS overrides
AGE_API void camPovCS::MakeActive()                           { hook::Thunk<0x51D530>::Call<void>(this); }
AGE_API void camPovCS::UpdateInput()                          { hook::Thunk<0x51D590>::Call<void>(this); }

//asNode overrides
AGE_API void camPovCS::Reset()                                { hook::Thunk<0x51D540>::Call<void>(this); }
AGE_API void camPovCS::Update()                               { hook::Thunk<0x51D570>::Call<void>(this); }
AGE_API void camPovCS::FileIO(datParser &parser)              { hook::Thunk<0x51D690>::Call<void>(this, &parser); }
AGE_API void camPovCS::AfterLoad()                            { hook::Thunk<0x51D710>::Call<void>(this); }
AGE_API char * camPovCS::GetClassName()                       { return hook::Thunk<0x51D760>::Call<char*>(this); }

//lua
void camPovCS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camPovCS, camCarCS>("camPovCS")
        .addFactory([]() {
        auto obj = new camPovCS();
        MM2Lua::MarkForCleanupOnShutdown(obj);
        return obj;
            })
        .addVariable("Offset", &camPovCS::Offset)
        .addVariable("ReverseOffset", &camPovCS::ReverseOffset)
        .addVariable("Pitch", &camPovCS::Pitch)
        .addVariable("POVJitterAmp", &camPovCS::POVJitterAmp)
    .endClass();
}

/*
    camTrackCS
*/

AGE_API void camTrackCS::Collide(Vector3 a1)                    { hook::Thunk<0x51EED0>::Call<void>(this, a1); }
AGE_API void camTrackCS::Front(float a1)                        { hook::Thunk<0x51F980>::Call<void>(this, a1); }
AGE_API void camTrackCS::MinMax(Matrix34 a1)                    { hook::Thunk<0x51ECC0>::Call<void>(this, a1); }
AGE_API void camTrackCS::PreApproach()                          { hook::Thunk<0x51EB40>::Call<void>(this); }
AGE_API void camTrackCS::Rear(float a1)                         { hook::Thunk<0x51FA00>::Call<void>(this, a1); }

AGE_API void camTrackCS::UpdateCar()                            { hook::Thunk<0x51DC70>::Call<void>(this); }
AGE_API void camTrackCS::UpdateHill()                           { hook::Thunk<0x51DEE0>::Call<void>(this); }
AGE_API void camTrackCS::UpdateTrack()                          { hook::Thunk<0x51E400>::Call<void>(this); }

AGE_API camTrackCS::camTrackCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x51D770>::Call<void>(this);
}

AGE_API camTrackCS::~camTrackCS(void)
{

}

AGE_API void camTrackCS::SwingToRear()                          { hook::Thunk<0x51F920>::Call<void>(this); }

bool camTrackCS::GetSteerOn() const     { return SteerOn == TRUE; }
void camTrackCS::SetSteerOn(bool value) { SteerOn = (value) ? TRUE : FALSE; }
bool camTrackCS::GetMinMaxOn() const    { return MinMaxOn == TRUE; }
void camTrackCS::SetMinMaxOn(bool value) { MinMaxOn = (value) ? TRUE : FALSE; }

//asNode overrides
AGE_API void camTrackCS::AfterLoad()                            { hook::Thunk<0x51DAF0>::Call<void>(this); }
AGE_API void camTrackCS::FileIO(datParser &parser)              { hook::Thunk<0x51FA80>::Call<void>(this, &parser); }
AGE_API char * camTrackCS::GetClassName()                       { return hook::Thunk<0x51FD60>::Call<char*>(this); }
AGE_API void camTrackCS::Reset()                                { hook::Thunk<0x51DB00>::Call<void>(this); }
AGE_API void camTrackCS::Update()                               { hook::Thunk<0x51DB50>::Call<void>(this); }

//camBaseCS overrides
AGE_API void camTrackCS::MakeActive()                           { hook::Thunk<0x51DAE0>::Call<void>(this); }
AGE_API void camTrackCS::UpdateInput()                          { hook::Thunk<0x51DC60>::Call<void>(this); }

//lua
void camTrackCS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camTrackCS, camCarCS>("camTrackCS")
        .addFactory([]() {
        auto obj = new camTrackCS();
        MM2Lua::MarkForCleanupOnShutdown(obj);
        return obj;
            })
        .addFunction("SwingToRear", &SwingToRear)
        .addVariable("Offset", &camTrackCS::Offset)
        .addVariable("CollideType", &camTrackCS::CollideType)
        .addProperty("MinMaxOn", &GetMinMaxOn, &SetMinMaxOn)
        .addVariable("TrackBreak", &camTrackCS::TrackBreak)
        .addVariable("MinAppXZPos", &camTrackCS::MinAppXZPos)
        .addVariable("MaxAppXZPos", &camTrackCS::MaxAppXZPos)
        .addVariable("MinSpeed", &camTrackCS::MinSpeed)
        .addVariable("MaxSpeed", &camTrackCS::MaxSpeed)
        .addVariable("AppInc", &camTrackCS::AppInc)
        .addVariable("AppDec", &camTrackCS::AppDec)
        .addVariable("MinHardSteer", &camTrackCS::MinHardSteer)
        .addVariable("DriftDelay", &camTrackCS::DriftDelay)
        .addVariable("VertOffset", &camTrackCS::VertOffset)
        .addVariable("FrontRate", &camTrackCS::FrontRate)
        .addVariable("RearRate", &camTrackCS::RearRate)
        .addVariable("FlipDelay", &camTrackCS::FlipDelay)
        .addProperty("SteerOn", &GetSteerOn, &SetSteerOn)
        .addVariable("SteerMin", &camTrackCS::SteerMin)
        .addVariable("SteerAmt", &camTrackCS::SteerAmt)
        .addVariable("HillMin", &camTrackCS::HillMin)
        .addVariable("HillMax", &camTrackCS::HillMax)
        .addVariable("HillLerp", &camTrackCS::HillLerp)
        .addVariable("RevDelay", &camTrackCS::RevDelay)
        .addVariable("RevOnApp", &camTrackCS::RevOnApp)
        .addVariable("RevOffApp", &camTrackCS::RevOffApp)
    .endClass();
}

/*
    camPolarCS
*/

AGE_API camPolarCS::camPolarCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x520FB0>::Call<void>(this);
}

AGE_API camPolarCS::~camPolarCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x406800>::Call<void>(this);
}

bool camPolarCS::GetAzimuthLock()
{
    return this->AzimuthLock == TRUE;
}

void camPolarCS::SetAzimuthLock(bool lock)
{
    this->AzimuthLock = (lock) ? TRUE : FALSE;
}

//asNode overrides
AGE_API void camPolarCS::FileIO(datParser &parser)              { hook::Thunk<0x521380 >::Call<void>(this, &parser); }
AGE_API char * camPolarCS::GetClassName()                       { return hook::Thunk<0x521460>::Call<char*>(this); }
AGE_API void camPolarCS::Reset()                                { hook::Thunk<0x521010>::Call<void>(this); }
AGE_API void camPolarCS::Update()                               { hook::Thunk<0x521030>::Call<void>(this); }

//camBaseCS overrides
AGE_API void camPolarCS::MakeActive()                           { hook::Thunk<0x521020>::Call<void>(this); }

//lua
void camPolarCS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camPolarCS, camCarCS>("camPolarCS")
        .addFactory([]() {
        auto obj = new camPolarCS();
        MM2Lua::MarkForCleanupOnShutdown(obj);
        return obj;
            })
        //properties
        .addProperty("AzimuthLock", &GetAzimuthLock, &SetAzimuthLock)
        .addVariable("Height", &camPolarCS::PolarHeight)
        .addVariable("Delta", &camPolarCS::PolarDelta)
        .addVariable("Distance", &camPolarCS::PolarDistance)
        .addVariable("Incline", &camPolarCS::PolarIncline)
        .addVariable("Azimuth", &camPolarCS::PolarAzimuth)
    .endClass();
}

/*
    camAICS
*/

AGE_API camAICS::camAICS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x520A30>::Call<void>(this);
}

AGE_API camAICS::~camAICS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x406810>::Call<void>(this);
}

float camAICS::getHeight(void)
{
    return *(float*)0x5D4340;
}

void camAICS::setHeight(float height)
{
    auto heightPtr = (float*)0x5D4340;
    *heightPtr = height;
}

AGE_API void camAICS::SetPosition(Vector3 *a1)               { hook::Thunk<0x520AC0>::Call<void>(this, a1); }
AGE_API void camAICS::SetMatrix(Matrix34 *a1)                { hook::Thunk<0x520AE0>::Call<void>(this, a1); }

// asNode overrides
AGE_API void camAICS::Reset()                                { hook::Thunk<0x520AB0>::Call<void>(this); }

//lua
void camAICS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camAICS, camCarCS>("camAICS")
        .addFactory([]() {
        auto obj = new camAICS();
        MM2Lua::MarkForCleanupOnShutdown(obj);
        return obj;
            })
        //properties
        .addStaticProperty("Height", &getHeight, &setHeight)
        .addVariable("Speed", &camAICS::Speed)
        .addVariable("RotationSpeed", &camAICS::RotationSpeed)
    .endClass();
}

/*
    camTransitionCS
*/

AGE_API camTransitionCS::camTransitionCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x521530>::Call<void>(this);
}

AGE_API camTransitionCS::~camTransitionCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x5215C0>::Call<void>(this);
}

// asNode overrides
AGE_API void camTransitionCS::Reset()                                { hook::Thunk<0x520AB0>::Call<void>(this); }
AGE_API void camTransitionCS::Update()                               { hook::Thunk<0x521610>::Call<void>(this); }

//lua
void camTransitionCS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camTransitionCS, camCarCS>("camTransitionCS")
        .endClass();
}

/*
    camViewCS
*/

//helpers for lua calls
bool camViewCS::NewCam(camCarCS* cam, int type, float time)
{
    return NewCam(cam, type, time, datCallback::NullCallback) == TRUE;
}

AGE_API camViewCS::camViewCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x51FD70>::Call<void>(this);
}

AGE_API camViewCS::~camViewCS(void)
{
    scoped_vtable x(this);
    hook::Thunk<0x51FE20>::Call<void>(this);
}

camCarCS * camViewCS::GetCurrentCamera(void) const             { return CurrentCamera; }
bool camViewCS::IsCurrentCamera(camCarCS *cam) const           { return CurrentCamera == cam; }
bool camViewCS::IsTransitioning() const                        { return IsCurrentCamera(this->TransitionCamera); }

AGE_API camViewCS * camViewCS::Instance(vehCar *a1)            { return hook::StaticThunk<0x51FE30>::Call<camViewCS *>(a1); }

AGE_API void camViewCS::SetCam(camCarCS *cam)                  { hook::Thunk<0x51FE90>::Call<void>(this, cam); }
AGE_API void camViewCS::OneShot()                              { hook::Thunk<0x5200C0>::Call<void>(this); }

AGE_API BOOL camViewCS::NewCam(camCarCS *cam, int type, float time, datCallback postTransitionCB)
                                                               { return hook::Thunk<0x51FF30>::Call<bool>(this, cam, type, time, postTransitionCB); }

AGE_API void camViewCS::Init(vehCar *car)                      { hook::Thunk<0x51FFF0>::Call<void>(this, car); }

AGE_API void camViewCS::ForceMatrixDelta(const Vector3 *a1)    { hook::Thunk<0x5200A0>::Call<void>(this, a1); }
AGE_API void camViewCS::ForceMatrixDelta(const Matrix34 *a1)   { hook::Thunk<0x520080>::Call<void>(this, a1); }

//overrides
AGE_API void camViewCS::Update()                               { hook::Thunk<0x51FFC0>::Call<void>(this); }
AGE_API void camViewCS::Reset()                                { hook::Thunk<0x520010>::Call<void>(this); }
AGE_API void camViewCS::FileIO(datParser &parser)              { hook::Thunk<0x5200D0>::Call<void>(this, &parser); }

void camViewCS::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<camViewCS, asNode>("camViewCS")
        //properties
        .addStaticFunction("Instance", &Instance)
        .addPropertyReadOnly("CurrentCamera", &GetCurrentCamera)
        .addPropertyReadOnly("IsTransitioning", &IsTransitioning)
        .addFunction("IsCurrentCamera", &IsCurrentCamera)
        .addFunction("SetCam", &SetCam)
        .addFunction("NewCam", static_cast<bool(camViewCS::*)(camCarCS*, int, float)>(&NewCam))
        .addFunction("Init", &Init)
        .addFunction("OneShot", &OneShot)
        .addFunction("ForceMatrixDelta", static_cast<void(camViewCS::*)(const Matrix34*)>(&ForceMatrixDelta))

        .addVariable("UseGlobalNearFar", &camViewCS::UseGlobalNearFar)
        .addVariable("FarClip", &camViewCS::FarClip)
        .addVariable("NearClip", &camViewCS::NearClip)
    .endClass();
}