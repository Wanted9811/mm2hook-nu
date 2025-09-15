#pragma once
#include "trackmanager.h"

using namespace MM2;

AGE_API lvlTrackManager::lvlTrackManager()
{
	hook::Thunk<0x465EC0>::Call<void>(this);
}

AGE_API lvlTrackManager::~lvlTrackManager()
{
	hook::Thunk<0x465ED0>::Call<void>(this);
}

AGE_API void lvlTrackManager::Init(Matrix34 const& wheelMatrix, float wheelWidth, int numTracks)
{
	hook::Thunk<0x465EE0>::Call<void>(this, &wheelMatrix, wheelWidth, numTracks);
}

AGE_API void lvlTrackManager::Reset()
{
	hook::Thunk<0x465F50>::Call<void>(this);
}

AGE_API void lvlTrackManager::Update(Vector3 const& intersecPoint, Vector3 const& intersecNormal, gfxTexture* tireTrackTexture)
{
	hook::Thunk<0x465F60>::Call<void>(this, &intersecPoint, &intersecNormal, tireTrackTexture);
}

AGE_API void lvlTrackManager::Draw()
{
	hook::Thunk<0x466320>::Call<void>(this);
}

AGE_API void lvlTrackManager::AddVertex(gfxTexture* tireTrackTexture, float textureWidth, Vector3 const& startPosition, Vector3 const& endPosition)
{
	hook::Thunk<0x4663E0>::Call<void>(this, tireTrackTexture, textureWidth, &startPosition, &endPosition);
}

void lvlTrackManager::BindLua(LuaState L) {
	LuaBinding(L).beginClass<lvlTrackManager>("lvlTrackManager")
		//functions
		.addFunction("Init", &Init)
		.addFunction("Reset", &Reset)
		.addFunction("Update", &Update)
		.addFunction("Draw", &Draw)
		.addFunction("AddVertex", &AddVertex)
	.endClass();
}