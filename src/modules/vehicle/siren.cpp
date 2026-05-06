#include "siren.h"
#include <modules\gfx\rstate.h>
#include <modules\data\timemgr.h>

using namespace MM2;

vehSiren::vehSiren()
{
    ltLight::GlowScale.set(0.2f);
    this->HasLights = false;
    this->Active = false;
    this->LightCount = 0;
    this->Light = nullptr;
    this->LightPositions = nullptr;
    this->RotationRate = 2.5f;
    this->LensFlare = nullptr;
    this->LightElectrics = nullptr;
    ltLight::GlowIntensity.set(0.6f);
}

vehSiren::~vehSiren()
{
    if (this->Light != nullptr)
    {
        ltLight::ShutdownLights();
        delete[] this->Light;
        this->Light = nullptr;
    }
    if (this->LightPositions != nullptr)
    {
        delete[] this->LightPositions;
        this->LightPositions = nullptr;
    }
    if (this->LightElectrics != nullptr)
    {
        delete[] this->LightElectrics;
        this->LightElectrics = nullptr;
    }
    if (this->LensFlare != nullptr)
    {
        delete this->LensFlare;
        this->LensFlare = nullptr;
    }
}

bool vehSiren::IsActive() const
{
    return Active;
}

void vehSiren::SetActive(bool active)
{
    Active = active;
}

bool vehSiren::GetHasLights() const
{
    return HasLights;
}

int vehSiren::GetLightCount() const
{
    return LightCount;
}

ltLight* vehSiren::GetLight(int index) const
{
    if (index < 0 || index >= LightCount)
        return nullptr;
    return &Light[index];
}

Vector3 vehSiren::GetLightPosition(int index) const
{
    if (index < 0 || index >= LightCount)
        return Vector3::ORIGIN;
    return LightPositions[index];
}

void vehSiren::SetLightPosition(int index, Vector3& position)
{
    if (index < 0 || index >= LightCount)
        return;
    LightPositions[index] = position;
}

bool vehSiren::GetLightElectric(int index)
{
    if (index < 0 || index >= LightCount)
        return false;
    return LightElectrics[index];
}

void vehSiren::SetLightElectric(int index, bool state)
{
    if (index < 0 || index >= LightCount)
        return;
    LightElectrics[index] = state;
}

//member funcs
AGE_API void vehSiren::Init(int lightCount)
{
    if (this->Light == nullptr)
    {
        this->Light = new ltLight[lightCount];
    }
    if (this->LightPositions == nullptr)
    {
        this->LightPositions = new Vector3[lightCount];
    }
    if (this->LightElectrics == nullptr)
    {
        this->LightElectrics = new bool[lightCount];
    }
    if (this->LensFlare == nullptr)
    {
        this->LensFlare = new ltLensFlare(20);
    }
}

void vehSiren::RemoveAllLights()
{
    this->HasLights = false;
    this->LightCount = 0;
}

AGE_API void vehSiren::AddLight(Vector3 const& position, Vector3 const& color)
{
    if (this->Light == nullptr)
        return;

    this->HasLights = true;
    Light[LightCount].Type = 1;
    Light[LightCount].Color = color;
    LightPositions[LightCount] = position;
    Light[LightCount].SpotExponent = 3.0f;
    Light[LightCount].Direction = Vector3(1.0f, 0.0f, 0.0f);
    Light[LightCount].Direction.RotateY(LightCount * 1.5707964f);
    ++this->LightCount;
}

AGE_API void vehSiren::Reset()
{
    this->Active = false;
}

AGE_API void vehSiren::Update()
{
    float rotationAmount = vehSiren::SirenRotationSpeed;

    if (this->Light != nullptr && this->Active)
    {
        for (int i = 0; i < this->LightCount; i++)
        {
            this->Light[i].Direction.RotateY(datTimeManager::Seconds * this->RotationRate * rotationAmount);
        }
    }
}

AGE_API void vehSiren::Draw(Matrix34 const& mtx)
{
    if (this->Light == nullptr)
        return;

    Vector3 camPosition = static_cast<Vector3>(gfxRenderState::GetCameraMatrix().GetRow(3));

    ltLight::DrawGlowBegin();
    for (int i = 0; i < this->LightCount; i++)
    {
        bool isSirenLightBroken = !(LightElectrics[i]);
        if (isSirenLightBroken)
            continue;

        auto light = &this->Light[i];
        auto lightPos = this->LightPositions[i];

        light->Position = mtx.Transform(lightPos);
        light->DrawGlow(camPosition);
    }
    ltLight::DrawGlowEnd();

    this->LensFlare->DrawBegin();
    for (int i = 0; i < this->LightCount; i++)
    {
        bool isSirenLightBroken = !(LightElectrics[i]);
        if (isSirenLightBroken)
            continue;

        auto light = &this->Light[i];
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
        .addFunction("GetLightElectric", &GetLightElectric)
        .addFunction("SetLightElectric", &SetLightElectric)

        //members
        .addFunction("Init", &Init)
        .addFunction("AddLight", &AddLight)

    .endClass();
}