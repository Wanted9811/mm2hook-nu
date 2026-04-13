#pragma once
#include <modules\vehicle.h>
#include <modules\audio\audimpact.h>

#include "nitrocaraudio.h"
#include "semicaraudio.h"
#include "policecaraudio.h"
#include "caraudio.h"


namespace MM2
{
    // Forward declarations
    class vehCarAudioContainer;

    // External declarations
    extern class Stream;

    // Class definitions

    class vehCarAudioContainer {
    private:
        BOOL HornPlaying;
        int Type;
        vehCarAudio* AudioPtr;
        vehPoliceCarAudio* PoliceAudioPtr;
        vehSemiCarAudio* SemiAudioPtr;
        vehNitroCarAudio* NitroAudioPtr;
    private:
        void silenceEngineLua(bool value);
        int getActiveAudioLua(lua_State* L);
    public:
        AGE_API static bool IsPolice(LPCSTR name);
        AGE_API static bool IsSemiOrBus(LPCSTR name);
        AGE_API static void RegisterSemiNames(Stream* a1, LPCSTR a2);
        AGE_API static void RegisterPoliceNames(Stream* a1, LPCSTR a2);
        AGE_API static void SetSirenCSVName(LPCSTR name);
        AGE_API void Set3D(bool a1);
        AGE_API void StartSiren();
        AGE_API void StopSiren();
        AGE_API void SilenceEngine(BOOL a1);
        AGE_API vehPoliceCarAudio* GetPoliceCarAudioPtr();
        AGE_API vehSemiCarAudio* GetSemiCarAudioPtr();
        AGE_API vehNitroCarAudio* GetNitroCarAudioPtr();
        AGE_API vehCarAudio* GetCarAudioPtr();
        AGE_API AudImpact* GetAudImpactPtr();
        AGE_API void PlayHorn();
        AGE_API void StopHorn();
        
        vehCarAudio* GetActiveAudio();

        bool GetIsHornPlaying();

        static bool IsEmergency(LPCSTR name);

        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(vehCarAudioContainer, 0x18);
}