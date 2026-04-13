#include "aiAmbientVehicleAudio.h"
#include <modules\vehicle\caraudiocontainer.h>

using namespace MM2;

/*
	aiAmbientVehicleAudio
*/

AGE_API aiAmbientVehicleAudio::aiAmbientVehicleAudio()
{
	scoped_vtable x(this);
	hook::Thunk<0x4CF890>::Call<void>(this);
}

AGE_API aiAmbientVehicleAudio::~aiAmbientVehicleAudio()
{
	scoped_vtable x(this);
	hook::Thunk<0x4CF8E0>::Call<void>(this);
}

/*
	fields
*/

bool aiAmbientVehicleAudio::GetSoundsLoaded() const
{
	return _SoundsLoaded.get(this);
}

void aiAmbientVehicleAudio::SetSoundsLoaded(bool state)
{
	_SoundsLoaded.set(this, state);
}

/*
    virtuals
*/

AGE_API void aiAmbientVehicleAudio::AssignSounds()
{
	hook::Thunk<0x4CF9D0>::Call<void>(this);
}

AGE_API void aiAmbientVehicleAudio::UnAssignSounds(int a1)
{
	hook::Thunk<0x4CFA50>::Call<void>(this, a1);
}

AGE_API void aiAmbientVehicleAudio::UpdateAudio()
{
	hook::Thunk<0x4CFB20>::Call<void>(this);
}

/*
    members
*/

AGE_API void aiAmbientVehicleAudio::Init(char* basename, Vector3* position)
{
	SetPositionPtr(position);
	SetDropOffs(0.0f, 100.0f);
	SetPriority(8);

	if (!GetSoundsLoaded())
	{
		bool isEmergency = vehCarAudioContainer::IsEmergency(basename);

		if (!LoadEngine(basename, "aud\\cardata\\ambient"))
			LoadEngine("default", "aud\\cardata\\ambient");

		if (!LoadHorn(basename, "aud\\cardata\\ambient"))
			LoadHorn(isEmergency ? "default_cop" : "default", "aud\\cardata\\ambient");

		LoadImpacts("default", "aud\\cardata\\opponent");

		if (!isEmergency)
		{
			if (!LoadVoices(basename, "aud\\creaturedata", false))
				LoadVoices("default", "aud\\creaturedata", true);
		}
		
		SetSoundsLoaded(true);
	}
}

AGE_API bool aiAmbientVehicleAudio::LoadEngine(char* basename, char* directory)
{
	return hook::Thunk<0x4D00D0>::Call<bool>(this, basename, directory);
}

AGE_API bool aiAmbientVehicleAudio::LoadHorn(char* basename, char* directory)
{
	return hook::Thunk<0x4D0300>::Call<bool>(this, basename, directory);
}

AGE_API bool aiAmbientVehicleAudio::LoadVoices(char* basename, char* directory, bool useVoiceFileNum)
{
	return hook::Thunk<0x4D0530>::Call<bool>(this, basename, directory, useVoiceFileNum);
}

AGE_API bool aiAmbientVehicleAudio::LoadImpacts(char* basename, char* directory)
{
	return hook::Thunk<0x4D07B0>::Call<bool>(this, basename, directory);
}