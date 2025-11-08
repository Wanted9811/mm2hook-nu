#pragma once
#include <modules\vehicle.h>
#include <modules\phys.h>
#include "carsim.h"
#include "drivetrain.h"
#include "wheel.h"
#include "trackmanager.h"

namespace MM2
{
    // Forward declarations
    class vehTrailer;
    class vehTrailerInstance;

    // External declarations
    extern class asNode;
    extern class dgPhysEntity;
    extern class dgTrailerJoint;
    extern class vehDrivetrain;
    extern class vehWheel;
    extern class lvlTrackManager;

    // Class definitions
    class vehTrailerInstance : public lvlInstance {
    public:
        static const int SHADOW_GEOM_ID = 1;
        static const int TLIGHT_GEOM_ID = 2;
        static const int TWHL0_GEOM_ID = 3;
        static const int TWHL1_GEOM_ID = 4;
        static const int TWHL2_GEOM_ID = 5;
        static const int TWHL3_GEOM_ID = 6;
        static const int HITCH_GEOM_ID = 7;
        static const int RLIGHT_GEOM_ID = 8;
        static const int BLIGHT_GEOM_ID = 9;
        static const int HLIGHT_GEOM_ID = 10;
        static const int SLIGHT0_GEOM_ID = 11;
        static const int SLIGHT1_GEOM_ID = 12;
        static const int SIREN0_GEOM_ID = 13;
        static const int SIREN1_GEOM_ID = 14;
        static const int TWHL4_GEOM_ID = 15;
        static const int TWHL5_GEOM_ID = 16;
        static const int TSWHL0_GEOM_ID = 17;
        static const int TSWHL1_GEOM_ID = 18;
        static const int TSWHL2_GEOM_ID = 19;
        static const int TSWHL3_GEOM_ID = 20;
        static const int TSWHL4_GEOM_ID = 21;
        static const int TSWHL5_GEOM_ID = 22;
        static const int TSLIGHT0_GEOM_ID = 23;
        static const int TSLIGHT1_GEOM_ID = 24;
    private:
        vehTrailer* Trailer;
        int Variant;
        Vector3 HitchOffset;
    private:
        Vector3 getTrailerHitchOffsetLua() const;
    public:
        Matrix34 GetWheelMatrix(int num);
        Matrix34 GetWheelShadowMatrix(int num, const Matrix34& shadowMatrix);
        vehTrailer* GetTrailer(void) const;

        void DrawPartReflected(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity, bool reflected);
        void DrawPartShadowed(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity);
        void DrawPart(int lod, int geomId, const Matrix34& matrix, modShader* shaders, bool alphaState);
        void DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders, bool alphaState);

        AGE_API void Init(const char* basename, Vector3 const& unused, int variant);

        /*
            lvlInstance virtuals
        */

        virtual AGE_API const Vector3& GetPosition() override;
        virtual AGE_API const Matrix34& GetMatrix(Matrix34& a1) override;
        virtual AGE_API void SetMatrix(const Matrix34& a1) override;
        virtual AGE_API void Draw(int lod) override;
        virtual AGE_API void DrawShadow() override;
        virtual AGE_API void DrawShadowMap() override;
        virtual AGE_API void DrawGlow() override;
        virtual AGE_API void DrawReflected(float intensity) override;
        virtual AGE_API void DrawReflectedParts(int lod) override;
        virtual AGE_API unsigned int SizeOf(void) override;

        /*
            vehTrailerInstance
        */

        AGE_API bool GetTrailerHitch(Vector3* out) const;

        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(vehTrailerInstance, 0x28);

    /*
        vehTrailer
    */

    class vehTrailer : public asNode, public dgPhysEntity {
    private:
        dgTrailerJoint TrailerJoint;
        vehTrailerInstance TrailerInstance;
        phInertialCS ICS;
        float Mass;
        Vector3 InertiaBox;
        Vector3 HitchOffsetDiff;
        Vector3 CarHitchOffset;
        Vector3 TrailerHitchOffset;
        vehDrivetrain TWHL0Drivetrain;
        vehDrivetrain TWHL1Drivetrain;
        vehDrivetrain TWHL2Drivetrain;
        vehDrivetrain TWHL3Drivetrain;
        vehWheel TWHL0;
        vehWheel TWHL1;
        vehWheel TWHL2;
        vehWheel TWHL3;
        lvlTrackManager TWHL0TrackManager;
        lvlTrackManager TWHL1TrackManager;
        lvlTrackManager TWHL2TrackManager;
        lvlTrackManager TWHL3TrackManager;
    public:
        vehCarSim* GetCarSim();
        phInertialCS* GetInertialCS();
        dgTrailerJoint* GetJoint(); 
        vehWheel* GetWheel(int num);
        lvlTrackManager* GetWheelTrackManager(int num);
        vehTrailerInstance* GetInstance();

        AGE_API vehTrailer();
        AGE_API ~vehTrailer();
        AGE_API void Init(const char* basename, const Vector3& carHitchOffset, vehCarSim* carSim, int variant);

        /*
            asNode virtuals
        */

        AGE_API void Reset() override;
        AGE_API void FileIO(datParser &parser) override;
        AGE_API bool Save() override;
        AGE_API bool Load() override;
        AGE_API char * GetClassName() override;
        AGE_API const char * GetDirName() override;

        /*
            dgPhysEntity virtuals
        */

        AGE_API void Update() override;
        AGE_API void PostUpdate() override;
        AGE_API phInertialCS * GetICS() override;
        AGE_API lvlInstance * GetInst() override;
        AGE_API bool RequiresTerrainCollision() override;

        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(vehTrailer, 0x1038);
}