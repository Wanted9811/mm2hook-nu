#include "aiTrafficLight.h"
#include <modules\lights\light.h>
#include <modules\city\citylevel.h>
#include <modules\mmcityinfo\state.h>

using namespace MM2;

/*
    aiTrafficLightSet
*/

void aiTrafficLightSet::SetCycleTime(float time)
{
    this->CycleTime = time;
}

float aiTrafficLightSet::GetCycleTime() const
{
    return this->CycleTime;
}

float aiTrafficLightSet::GetTimeInCycle() const
{
    return this->Timer;
}

int aiTrafficLightSet::GetCurrentLightIdx() const
{
    return this->CurrentLightIdx;
}

int aiTrafficLightSet::GetLightCount() const
{
    return this->LightCount;
}

aiTrafficLightInstance* aiTrafficLightSet::GetLight(int num)
{
    if (num < 0 || num >= this->GetLightCount())
        return nullptr;
    return this->TrafficLights[num];
}

void aiTrafficLightSet::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<aiTrafficLightSet, asNode>("aiTrafficLightSet")
        .addPropertyReadOnly("NumLights", &GetLightCount)
        .addPropertyReadOnly("CurrentLightIdx", &GetCurrentLightIdx)
        .addProperty("CycleTime", &GetCycleTime, &SetCycleTime)
        .addPropertyReadOnly("TimeInCycle", &GetTimeInCycle)
        .addFunction("GetLight", &GetLight)
    .endClass();
}

/*
    aiTrafficLightInstance
*/

int aiTrafficLightInstance::InitLua(LPCSTR name, Vector3 const& position, Vector3 const& positionFacing)
{
    return this->Init(name, position, positionFacing, 0, nullptr);
}

void aiTrafficLightInstance::DrawGlowShadow()
{
	//return if there's no dgBangerData file found
	auto data = this->GetData();
	if (!data->isActive())
		return;

	//get glow shadow textures
	gfxTexture* redGlowShadow = gfxGetTexture("s_red_glow_shadow", true);
	gfxTexture* yelGlowShadow = gfxGetTexture("s_yel_glow_shadow", true);
	gfxTexture* grnGlowShadow = gfxGetTexture("s_grn_glow_shadow", true);

	//no need to draw glow shadows if they don't exist
	if (redGlowShadow == NULL && yelGlowShadow == NULL && grnGlowShadow == NULL)
		return;
	if (dgBangerInstance::DefaultGlowTexture.get() == NULL)
		return;

	//get matrix
	Matrix34 dummyMatrix, shadowMatrix = Matrix34();
	Matrix34 trafLightMatrix = this->GetMatrix(dummyMatrix);

	//get data stuff
	bool isDoubled = strstr(data->GetName(), "dual");
	bool isCentered = strstr(data->GetName(), "_l");
	float heightDifference = (data->CG.Y * 2.0f) - (data->CG.Y * 0.1f);
	float movementMultiplier = 1.0f;

	//draw light shadows
	gfxRenderState::SetFogEnabled(false);
	gfxRenderState::SetBlendSet(7);
	rglWorldIdentity();
	vglBeginBatch();

	int state = this->GetState();
	if (state == 1 || state == 4 || state == 5) // REDLIGHT
	{
		heightDifference += 1.5f;
		vglBindTexture(redGlowShadow);
	}
	if (state == 2) // YELLOWLIGHT
	{
		heightDifference += 1.0f;
		vglBindTexture(yelGlowShadow);
	}
	if (state == 3) // GREENLIGHT
	{
		heightDifference += 0.5f;
		vglBindTexture(grnGlowShadow);
	}

	isCentered ? movementMultiplier = 0.0f : movementMultiplier = 0.5f;

	trafLightMatrix.SetRow(3, trafLightMatrix.GetRow(3) + (trafLightMatrix.GetRow(2) * heightDifference) - (trafLightMatrix.GetRow(0) * heightDifference * movementMultiplier));

	float size = fmaxf(heightDifference, 0.0f);
	float intensity = fminf(fmaxf(heightDifference * 0.1f, 0.0f), 1.0f);

	isCentered ? intensity *= 1.5f : intensity *= 1.0f;
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f) * (1.0f - intensity);

	if (lvlInstance::ComputeShadowMatrix(shadowMatrix, this->GetRoomId(), trafLightMatrix))
	{
		shadowMatrix.m31 += 0.15f; //sidewalk height
		tglDrawShadowedParticle(shadowMatrix, size, color);
	}

	if (isDoubled)
	{
		Matrix34 trafLightMatrix = this->GetMatrix(dummyMatrix);
		trafLightMatrix.SetRow(3, trafLightMatrix.GetRow(3) + (trafLightMatrix.GetRow(2) * heightDifference) - (trafLightMatrix.GetRow(0) * heightDifference * 1.25f));

		if (lvlInstance::ComputeShadowMatrix(shadowMatrix, this->GetRoomId(), trafLightMatrix))
		{
			shadowMatrix.m31 += 0.15f; //sidewalk height
			tglDrawShadowedParticle(shadowMatrix, size, color);
		}
	}

	vglEndBatch();
	gfxRenderState::SetBlendSet(0);
	gfxRenderState::SetFogEnabled(!useSoftware);
}

aiTrafficLightInstance::aiTrafficLightInstance()
{
    // aiTrafficLightInstance ctor got inlined
    // but the inlined constructor implies flag 1 was set in it
    this->SetFlags(this->GetFlags() | 0x1);
}

/*
    lvlInstance virtuals
*/

AGE_API void aiTrafficLightInstance::Draw(int lod)                         { hook::Thunk<0x53C960>::Call<void>(this, lod); }

AGE_API void aiTrafficLightInstance::DrawShadow()
{
	if (dgBangerInstance::LightShadows)
		DrawGlowShadow();

	if (MMSTATE->TimeOfDay == 3 ||
		MMSTATE->WeatherType != 0 ||
		lvlLevel::GetSingleton()->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
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

AGE_API void aiTrafficLightInstance::DrawGlow()                            { hook::Thunk<0x53CA70>::Call<void>(this); }
AGE_API unsigned int aiTrafficLightInstance::SizeOf(void)                  { return hook::Thunk<0x53CD50>::Call<unsigned int>(this); }

/* 
    aiTrafficLight virtuals
*/

AGE_API int aiTrafficLightInstance::Init(LPCSTR name, Vector3 const & position, Vector3 const & positionFacing, int unused_a5, LPCSTR unused_a6)
                                                                   { return hook::Thunk<0x53C6F0>::Call<int>(this, name, &position, &positionFacing, unused_a5, unused_a6); }

int aiTrafficLightInstance::GetState() const
{
    return State;
}

void aiTrafficLightInstance::SetState(int state)
{
    this->State = state;
}

void aiTrafficLightInstance::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<aiTrafficLightInstance, dgUnhitYBangerInstance>("aiTrafficLightInstance")
        .addFactory([]() {
        return new aiTrafficLightInstance();
            })
        .addFunction("Init", &InitLua)
        .addProperty("State", &GetState, &SetState)
    .endClass();
}