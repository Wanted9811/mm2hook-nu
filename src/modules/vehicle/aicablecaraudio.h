#pragma once
#include <modules\vehicle.h>
#include "aicablecaraudiodata.h"

namespace MM2
{
    // Forward declarations
    class aiCableCarAudio;

    // External declarations
    extern class aiCableCarAudioData;

    // Class definitions
    class aiCableCarAudio : public Aud3DObject
    {
        aiCableCarAudioData AudioData;
    };
    ASSERT_SIZEOF(aiCableCarAudio, 0x94);

    // Lua initialization

}