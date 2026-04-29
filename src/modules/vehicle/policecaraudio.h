#pragma once
#include "caraudio.h"

namespace MM2
{
    // Forward declarations
    class vehPoliceCarAudio;

    // External declarations


    // Class definitions

    class vehPoliceCarAudio : public vehCarAudio {
    private:
		byte _buffer[0x118];
    protected:
		static hook::Field<0x138, AudSoundBase*> _explosionSound;
    public:
        static hook::Type<int> iNumCopsPursuingPlayer;

        AudSoundBase* GetExplosionSound() const
        {
            return _explosionSound.get(this);
		}

        AGE_API void StartSiren(int a1)                   { hook::Thunk<0x4D4B20>::Call<void>(this, a1); }
        AGE_API void StopSiren()                          { hook::Thunk<0x4D4C20>::Call<void>(this); }
        AGE_API void PlayExplosion()                      { hook::Thunk<0x4D4C90>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehPoliceCarAudio, vehCarAudio>("vehPoliceCarAudio")
                .addFunction("StartSiren", &StartSiren)
                .addFunction("StopSiren", &StopSiren)
                .addFunction("PlayExplosion", &PlayExplosion)
                .endClass();
        }
    };
	ASSERT_SIZEOF(vehPoliceCarAudio, 0x248);
}