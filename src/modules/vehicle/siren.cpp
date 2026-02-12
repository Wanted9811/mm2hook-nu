#include "siren.h"
#include <modules\gfx\rstate.h>
#include <modules\data\timemgr.h>

using namespace MM2;

bool vehSiren::IsActive() const      { return Active;}
void vehSiren::SetActive(bool active) { Active = active; }
bool vehSiren::GetHasLights() const  { return HasLights; }
int vehSiren::GetLightCount() const  { return LightCount; }

ltLight* vehSiren::GetLight(int index) const
{
    if (index < 0 || index >= vehSiren::MAX_LIGHTS)
        return nullptr;
    return &ltLightPool[index];
}

Vector3 vehSiren::GetLightPosition(int index) const
{
    if (index < 0 || index >= vehSiren::MAX_LIGHTS)
        return Vector3::ORIGIN;
    return extraLightPositions[index];
}

void vehSiren::SetLightPosition(int index, Vector3 position)
{
    if (index < 0 || index >= vehSiren::MAX_LIGHTS)
        return;
    extraLightPositions[index] = position;
}

void vehSiren::SetEnabledElectrics(int index, bool state)
{
    enabledElectrics[index] = state;
}

//member funcs
AGE_API bool vehSiren::Init()
{
    if (this->ltLightPool == nullptr)
    {
        this->ltLightPool = new ltLight[vehSiren::MAX_LIGHTS];
    }
    if (this->LensFlare == nullptr)
    {
        this->LensFlare = new ltLensFlare(20);
    }
    return true;
}

void vehSiren::RemoveAllLights()
{
    this->HasLights = false;
    this->LightCount = 0;
}

AGE_API bool vehSiren::AddLight(Vector3 const& position, Vector3 const& color)
{
    if (this->ltLightPool != nullptr && this->LightCount < vehSiren::MAX_LIGHTS)
    {
        this->HasLights = true;
        ltLightPool[LightCount].Type = 1;
        ltLightPool[LightCount].Color = color;
        extraLightPositions[LightCount] = position;
        ltLightPool[LightCount].SpotExponent = 3.f;
        ltLightPool[LightCount].Direction = Vector3(1.0f, 0.0f, 0.0f);
        ltLightPool[LightCount].Direction.RotateY(LightCount * 1.5707964f);
        ++this->LightCount;
        return true;
    }

    return false;
}

AGE_API void vehSiren::Reset()
{
    this->Active = false;
}

AGE_API void vehSiren::Update()
{
    float rotationAmount = vehSiren::SirenRotationSpeed;

    if (this->ltLightPool != nullptr && this->Active)
    {
        for (int i = 0; i < this->LightCount; i++)
        {
            this->ltLightPool[i].Direction.RotateY(datTimeManager::Seconds * this->RotationRate * rotationAmount);
        }
    }
}

AGE_API void vehSiren::Draw(Matrix34 const& mtx)
{
    if (this->ltLightPool == nullptr)
        return;

    Vector3 camPosition = static_cast<Vector3>(gfxRenderState::GetCameraMatrix().GetRow(3));

    ltLight::DrawGlowBegin();
    for (int i = 0; i < this->LightCount; i++)
    {
        bool isSirenLightBroken = !(enabledElectrics[i]);
        if (isSirenLightBroken)
            continue;

        auto light = &this->ltLightPool[i];
        auto lightPos = this->extraLightPositions[i];

        light->Position = mtx.Transform(lightPos);
        light->DrawGlow(camPosition);
    }
    ltLight::DrawGlowEnd();

    this->LensFlare->DrawBegin();
    for (int i = 0; i < this->LightCount; i++)
    {
        bool isSirenLightBroken = !(enabledElectrics[i]);
        if (isSirenLightBroken)
            continue;

        auto light = &this->ltLightPool[i];
        float intensity = light->ComputeIntensity(camPosition, 0.05f);
        this->LensFlare->Draw(light->Position, light->Color, intensity);
    }
    this->LensFlare->DrawEnd();
}

void vehSiren::BindLua(LuaState L) {
    LuaBinding(L).beginClass<vehSiren>("vehSiren")
        //variables
        .addPropertyReadOnly("HasLights", &GetHasLights)
        .addPropertyReadOnly("LightCount", &GetLightCount)
        .addProperty("Active", &IsActive, &SetActive)
        .addVariable("RotationRate", &vehSiren::RotationRate)

        //lua members
        .addFunction("GetLight", &GetLight)
        .addFunction("GetLightPosition", &GetLightPosition)
        .addFunction("SetLightPosition", &SetLightPosition)

        //members
        .addFunction("Init", &Init)
        .addFunction("AddLight", &AddLight)

    .endClass();
}