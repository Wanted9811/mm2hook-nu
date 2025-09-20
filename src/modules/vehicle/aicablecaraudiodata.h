#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class aiCableCarAudioData;

    // External declarations


    // Class definitions
    class aiCableCarAudioData
    {
    private:
        AudSoundBase* GoBellSoundBasePtr;
        AudSoundBase* StopSoundBasePtr;
        AudSoundBase* LoopSoundBasePtr;
        AudSoundBase* StartSoundBasePtr;
        AudSoundBase* CrossIntersectionSoundBasePtr;
        AudSoundBase* StreetCableSoundBasePtr;
        int GoBellSampleIndex;
        int StopSampleIndex;
        int LoopSampleIndex;
        int StartSampleIndex;
        int StreetCableSampleIndex;
        int State;
        int LastState;
    };
    ASSERT_SIZEOF(aiCableCarAudioData, 0x34);

    // Lua initialization

}