#pragma once
#include <modules\audio\aud3dobject.h>

namespace MM2
{
    // Forward declarations
    class aiAmbientVehicleAudio;

    // External declarations


    // Class definitions

    class aiAmbientVehicleAudio : public Aud3DObject {
    private:
        byte _buffer[0x2C];
    protected:
		static hook::Field<0x88, bool> _SoundsLoaded;
    public:
		AGE_API aiAmbientVehicleAudio();
		AGE_API ~aiAmbientVehicleAudio();

        //fields
        bool GetSoundsLoaded() const;
        void SetSoundsLoaded(bool state);

        //virtuals
        AGE_API void AssignSounds() override;
        AGE_API void UnAssignSounds(int a1) override;
        AGE_API void UpdateAudio() override;

        //members
		AGE_API void Init(char* basename, Vector3* position);
		AGE_API bool LoadEngine(char* basename, char* directory);
		AGE_API bool LoadHorn(char* basename, char* directory);
		AGE_API bool LoadVoices(char* basename, char* directory, bool useVoiceFileNum);
		AGE_API bool LoadImpacts(char* basename, char* directory);
    };
    ASSERT_SIZEOF(aiAmbientVehicleAudio, 0x8C);

    // Lua initialization

}