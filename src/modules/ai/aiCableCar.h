#pragma once
#include <mm2_common.h>
#include "aiVehicle.h"
#include "aiPath.h"
#include "aiRailSet.h"
#include <modules\banger\banger.h>
#include <modules\vehicle\aicablecaraudio.h>

namespace MM2
{
    // Forward declarations
    class aiCableCar;
    class aiCableCarInstance;

    // External declarations
    extern class aiRailSet;
    extern class aiCableCarAudio;
    extern class dgUnhitBangerInstance;

    // Class definitions

    class aiCableCar : public aiVehicle {
    private:
        aiCableCarInstance* Instance;
        Matrix34* Matrix;
        Vector3 Velocity;
        lvlSegmentInfo SegmentInfo1;
        lvlSegmentInfo SegmentInfo2;
        lvlSegmentInfo SegmentInfo3;
        aiRailSet RailSet;
        aiCableCarAudio Audio;
        int field_17C;
        int field_180;
    public:
        aiCableCarInstance* GetInstance();

        AGE_API aiCableCar();
        AGE_API ~aiCableCar();

        /*
            aiObstacle virtuals
        */

        AGE_API void Position(Vector3 &position) override;
        AGE_API float Speed() override;
        AGE_API int CurrentRoadIdx(aiPath **a1, const bool *a2, int *a3) override;
        AGE_API int CurrentRdVert() override;

        /*
            aiVehicle virtuals
        */

        AGE_API void Update() override;
        AGE_API void Reset() override;
        AGE_API int Type() override;
        AGE_API Matrix34 & GetMatrix() const override;
        AGE_API float FrontBumperDistance() override;
        AGE_API float BackBumperDistance() override;
        AGE_API float LSideDistance() override;
        AGE_API float RSideDistance() override;
        AGE_API int CurrentLane() override;
        AGE_API int CurrentRoadId() override;
        AGE_API void DrawId() const override;

        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(aiCableCar, 0x184);

    /*
        aiCableCarInstance
    */

    class aiCableCarInstance : public dgUnhitBangerInstance
    {
    public:
        /*
            Model Index Constants
        */
        static const int SHADOW_GEOM_ID = 1;
        static const int HLIGHT_GEOM_ID = 2;
    private:
        int field_18;
        int field_1C;
        int field_20;
        int field_24;
        int field_28;
        int field_2C;
        int field_30;
        int field_34;
        int field_38;
        aiCableCar* CableCar;
        dgHitBangerInstance* HitBangInst;
    public:
        aiCableCar* GetCableCar();

        /*
            lvlInstance virtuals
        */

        AGE_API Vector3 const & GetPosition() override;
        AGE_API Matrix34 const & GetMatrix(Matrix34 &matrix) override;
        AGE_API void SetMatrix(const Matrix34 &matrix) override;
        AGE_API Vector3 const & GetVelocity() override;
        AGE_API void Draw(int lod) override;
        AGE_API void DrawShadow() override;
        AGE_API void DrawGlow() override;
        AGE_API void DrawReflected(float intensity) override;
        AGE_API void DrawReflectedParts(int lod) override;
        AGE_API unsigned int SizeOf() override;

        /* 
            dgBangerInstance virtuals
        */

        AGE_API void ImpactCB(dgHitBangerInstance* hitBangInst) override;

        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(aiCableCarInstance, 0x44);

    // Lua initialization

}