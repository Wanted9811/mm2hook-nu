#pragma once
#include <modules\phys\physentity.h>
#include <modules\vehicle\cardamage.h>
#include <modules\vehicle\carsim.h>
#include <modules\vehicle\splash.h>
#include <modules\vehicle\caraudiocontainer.h>
#include <modules\vehicle\siren.h>
#include <modules\vehicle\stuck.h>
#include <modules\vehicle\gyro.h>
#include <modules\vehicle\carmodel.h>
#include <modules\vehicle\trailer.h>
#include <modules\vehicle\trackmanager.h>

namespace MM2
{
    // Forward declarations
    class vehCar;

    // External declarations
    extern class dgPhysEntity;
    extern class lvlInstance;
    extern class phInertialCS;

    extern class vehCarAudioContainer;
    extern class vehCarDamage;
    extern class vehCarModel;
    extern class vehCarSim;
    extern class vehSplash;
    extern class vehTrailer;
    extern class vehSiren;
    extern class vehGyro;
    extern class vehStuck;
    extern class vehWheelPtx;
    extern class lvlTrackManager;

    // Class definitions

    class vehCar : public dgPhysEntity {
    private:
        void* Input;
        vehCarSim* CarSim;
        vehCarModel* CarModel;
        vehCarDamage* CarDamage;
        vehWheelPtx* WheelPtx;
        vehSiren* Siren;
        vehStuck* Stuck;
        vehGyro* Gyro;
        int field_D4;
        vehTrailer* Trailer;
        void* Driver;
        vehSplash* Splash;
        void* Feedback;
        int Flags;
        int StopMode;
        gfxTexture* TireTrackTexture;
        lvlTrackManager WHL0TrackManager;
        lvlTrackManager WHL1TrackManager;
        lvlTrackManager WHL2TrackManager;
        lvlTrackManager WHL3TrackManager;
        vehCarAudioContainer* CarAudioContainer;
    private:
        /*
            Valid Modes:
            1 - Forced neutral transmission. Brake input is forced to 1.0.
            2 - Brake input is forced to 1.0. Other inputs are cleared.
            3 - Like mode 2, additionally neutral transmission set on function call, but not enforced. 
        */
        void setDrivable(bool drivable, int mode);
        phColliderJointed* getColliderLua() { return (phColliderJointed*)this->GetCollider(); }
    public:
        ANGEL_ALLOCATOR

        AGE_API vehCar(BOOL a1);
        AGE_API ~vehCar();

        static hook::Type<bool> sm_DrawHeadlights;

        vehCarDamage* GetCarDamage(void) const;
        vehCarSim* GetCarSim() const;
        vehCarModel* GetModel() const;
        vehSplash* GetSplash() const;
        vehCarAudioContainer* GetCarAudioContainerPtr() const;
        vehTrailer* GetTrailer() const;
        vehSiren* GetSiren() const;
        vehWheelPtx* GetWheelPtx() const;
        vehGyro* GetGyro() const;
        vehStuck* GetStuck() const;
        lvlTrackManager* GetWheelTrackManager(int num);

        AGE_API void Reset();

        AGE_API void ClearDamage();
        AGE_API bool IsPlayer();
        AGE_API void Init(char const* basename, int variant, int colliderID, bool useFullBound, bool hasTrailer);
        AGE_API void InitAudio(char const* basename, int audioType);
        AGE_API void SetDrivable(BOOL drivable, int mode);
        AGE_API void SetColliderID(int id);
        AGE_API void DrawTracks();

        /*
            dgPhysEntity virtuals
        */

        AGE_API bool RequiresTerrainCollision() override;
                                                       
        AGE_API lvlInstance* GetInst() override;
        AGE_API phInertialCS* GetICS();
        AGE_API void PostUpdate() override;
        AGE_API void Update() override;
        AGE_API void PreUpdate() override;

        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(vehCar, 0x258);
}