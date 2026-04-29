#include "caraudiocontainer.h"

using namespace MM2;

void vehCarAudioContainer::silenceEngineLua(bool value)
{
    this->SilenceEngine(value ? TRUE : FALSE);
}

int vehCarAudioContainer::getActiveAudioLua(lua_State* L)
{
    if (AudioPtr != nullptr)
        LuaState(L).push(AudioPtr);
    else if (PoliceAudioPtr != nullptr)
        LuaState(L).push(PoliceAudioPtr);
    else if (SemiAudioPtr != nullptr)
        LuaState(L).push(SemiAudioPtr);
    else if (NitroAudioPtr != nullptr)
        LuaState(L).push(NitroAudioPtr);
    else
        LuaState(L).push(nullptr);
    return 1;
}

AGE_API bool vehCarAudioContainer::IsPolice(LPCSTR name)                      { return hook::StaticThunk<0x4D1A70>::Call<bool>(name); }
AGE_API bool vehCarAudioContainer::IsSemiOrBus(LPCSTR name)                   { return hook::StaticThunk<0x4D1A00>::Call<bool>(name); }
AGE_API void vehCarAudioContainer::RegisterSemiNames(Stream* a1, LPCSTR a2)   { hook::StaticThunk<0x4D0E30>::Call<void>(a1, a2); }
AGE_API void vehCarAudioContainer::RegisterPoliceNames(Stream* a1, LPCSTR a2) { hook::StaticThunk<0x4D0F10>::Call<void>(a1, a2); }
AGE_API void vehCarAudioContainer::SetSirenCSVName(LPCSTR name)               { hook::StaticThunk<0x4D0C80>::Call<void>(name); }
AGE_API void vehCarAudioContainer::Set3D(bool a1)                             { hook::Thunk<0x4D1840>::Call<void>(this, a1); }

AGE_API void vehCarAudioContainer::StartSiren()
{
    auto policeAudio = this->GetPoliceCarAudioPtr();
    if (policeAudio != nullptr)
    {
        policeAudio->StartSiren(0);
	}
}

AGE_API void vehCarAudioContainer::StopSiren()
{
    auto policeAudio = this->GetPoliceCarAudioPtr();
    if (policeAudio != nullptr)
    {
        policeAudio->StopSiren();
	}
}

AGE_API void vehCarAudioContainer::SilenceEngine(BOOL a1)                     { hook::Thunk<0x4D1910>::Call<void>(this, a1); }
AGE_API vehPoliceCarAudio* vehCarAudioContainer::GetPoliceCarAudioPtr()       { return hook::Thunk<0x4D1790>::Call<vehPoliceCarAudio*>(this); }
AGE_API vehSemiCarAudio* vehCarAudioContainer::GetSemiCarAudioPtr()           { return hook::Thunk<0x4D17A0>::Call<vehSemiCarAudio*>(this); }
AGE_API vehNitroCarAudio* vehCarAudioContainer::GetNitroCarAudioPtr()         { return hook::Thunk<0x4D1780>::Call<vehNitroCarAudio*>(this); }
AGE_API vehCarAudio* vehCarAudioContainer::GetCarAudioPtr()                   { return hook::Thunk<0x4D1770>::Call<vehCarAudio*>(this); }
AGE_API AudImpact* vehCarAudioContainer::GetAudImpactPtr()                    { return hook::Thunk<0x4D1730>::Call<AudImpact*>(this); }

AGE_API void vehCarAudioContainer::PlayHorn()
{
    // rewritten to support police audio horns
    // MM2 only calls stop horn on base/nitro/semi audio
    if (this->GetIsHornPlaying())
        return;

    auto activeAudio = GetActiveAudio();
    if (activeAudio != nullptr) {
        activeAudio->PlayHorn();
    }
    this->HornPlaying = TRUE;
}

AGE_API void vehCarAudioContainer::StopHorn()
{
    // rewritten to support police audio horns
    // MM2 only calls stop horn on base/nitro/semi audio
    if (!this->GetIsHornPlaying())
        return;

    auto activeAudio = GetActiveAudio();
    if (activeAudio != nullptr) {
        activeAudio->StopHorn();
    }
    this->HornPlaying = FALSE;
}
        
vehCarAudio* vehCarAudioContainer::GetActiveAudio()
{
    if (AudioPtr != nullptr)
        return AudioPtr;
    else if (PoliceAudioPtr != nullptr)
        return PoliceAudioPtr;
    else if (SemiAudioPtr != nullptr)
        return SemiAudioPtr;
    else if (NitroAudioPtr != nullptr)
        return NitroAudioPtr;
    return nullptr;
}

bool vehCarAudioContainer::GetIsHornPlaying()
{
    return HornPlaying == TRUE;
}

bool vehCarAudioContainer::IsEmergency(LPCSTR name)
{
    // We excluded vpauditt name from adding it as an emergency vehicle (non-police) because it has leftover siren files in geometry folder
    string_buf<256> buffer("%s_srn0", name);
    return (IsPolice(name) || (_strcmpi(name, "vpauditt") && datAssetManager::Exists("geometry", buffer, "mtx"))) != false;
}

void vehCarAudioContainer::BindLua(LuaState L) {
    LuaBinding(L).beginClass<vehCarAudioContainer>("vehCarAudioContainer")
        .addStaticFunction("IsPolice", &IsPolice)
        .addStaticFunction("IsEmergency", &IsEmergency)
        .addStaticFunction("IsSemiOrBus", &IsSemiOrBus)
        .addStaticFunction("SetSirenCSVName", &SetSirenCSVName)
        .addPropertyReadOnly("HornPlaying", &GetIsHornPlaying)
        .addFunction("GetActiveAudio", &getActiveAudioLua)
        .addFunction("Set3D", &Set3D)
        .addFunction("StartSiren", &StartSiren)
        .addFunction("StopSiren", &StopSiren)
        .addFunction("PlayHorn", &PlayHorn)
        .addFunction("StopHorn", &StopHorn)
        .addFunction("SilenceEngine", &silenceEngineLua)
    .endClass();
}