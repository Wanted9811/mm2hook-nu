#pragma once
#include <mm2_common.h>
#include <modules\level\inst.h>
#include <modules\vehicle\breakable.h>
#include "aiRailSet.h"

namespace MM2
{
    // Forward declarations
    class aiObstacle;
    class aiObstacleLuaIterator;
    class aiVehicle;
    class aiVehicleSpline;
    class aiVehicleInstance;
    
    // External declarations
    extern class aiPath;
    extern class aiMap;
    extern class aiVehicleInstance;
    extern class dgPhysEntity;
    extern class phBound;
    extern class aiVehicleData;

    // Class definitions
    class aiVehicleInstance : public lvlInstance {
    public:
        static const int SHADOW_GEOM_ID = 1;
        static const int HLIGHT_GEOM_ID = 2;
        static const int TLIGHT_GEOM_ID = 3;
        static const int SLIGHT0_GEOM_ID = 4;
        static const int SLIGHT1_GEOM_ID = 5;
        static const int WHL0_GEOM_ID = 6;
        static const int WHL1_GEOM_ID = 7;
        static const int WHL2_GEOM_ID = 8;
        static const int WHL3_GEOM_ID = 9;
        static const int BREAK0_GEOM_ID = 10;
        static const int BREAK1_GEOM_ID = 11;
        static const int BREAK2_GEOM_ID = 12;
        static const int BREAK3_GEOM_ID = 13;
        static const int HEADLIGHT0_GEOM_ID = 14;
        static const int HEADLIGHT1_GEOM_ID = 15;
        static const int WHL4_GEOM_ID = 16;
        static const int WHL5_GEOM_ID = 17;
        static const int BLIGHT_GEOM_ID = 18;
        static const int PLIGHTON_GEOM_ID = 19;
        static const int PLIGHTOFF_GEOM_ID = 20;
        static const int TSLIGHT0_GEOM_ID = 21;
        static const int TSLIGHT1_GEOM_ID = 22;
        static const int SWHL0_GEOM_ID = 23;
        static const int SWHL1_GEOM_ID = 24;
        static const int SWHL2_GEOM_ID = 25;
        static const int SWHL3_GEOM_ID = 26;
        static const int SWHL4_GEOM_ID = 27;
        static const int SWHL5_GEOM_ID = 28;
    public:
        static int AmbientHeadlightStyle;
    private:
        aiVehicleSpline* Spline;
        short RandomId;
        byte SignalState;
        short LOD;
        short Variant;
        vehBreakableMgr* BreakableMgr;
        Vector3 HeadlightPosition;
        Vector3 VehiclePosition;
    public:
        aiVehicleInstance(void)             DONOTCALL;

        //properties
        aiVehicleSpline* GetSpline();
        int GetVariant() const;
        
        //overrides
        AGE_API Vector3 const& GetPosition() override;
        AGE_API Matrix34 const& GetMatrix(Matrix34& a1) override;
        AGE_API void SetMatrix(Matrix34 const& a1) override;
        AGE_API void SetVariant(int variant) override;
        AGE_API dgPhysEntity* GetEntity() override;
        AGE_API dgPhysEntity* AttachEntity() override;
        AGE_API void Detach() override;
        AGE_API void Draw(int lod) override;
        AGE_API void DrawShadow() override;
        AGE_API void DrawShadowMap() override;
        AGE_API void DrawGlow() override;        
        AGE_API void DrawReflected(float intensity) override;
        AGE_API void DrawReflectedParts(int lod) override;
        AGE_API unsigned int SizeOf() override;
        AGE_API phBound* GetBound(int a1) override;
        
        //members
        vehBreakableMgr* GetGenBreakableMgr();
        aiVehicleData* GetData();
        AGE_API void DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders, int unused);

        //helpers
        void DrawPartReflected(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity, bool reflected);
        void DrawPartShadowed(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity);
        void DrawPart(int lod, int geomId, const Matrix34& matrix, modShader* shaders, bool alphaState);
        void DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders, bool alphaState);
        void DrawHeadlights();
        Matrix34 GetWheelMatrix(int num);
        Matrix34 GetWheelShadowMatrix(int num, const Matrix34& shadowMatrix);

        //lua
        static void BindLua(LuaState L);
    };

    class aiObstacle {
    private:
        aiObstacle* Next;
    private:
        bool luaDrivable() { return Drivable() == TRUE; }
        bool luaInAccident() { return InAccident() == TRUE; }
        Vector3 luaPosition() { Vector3 p = Vector3::ORIGIN; this->Position(p); return p; }
    public:
        aiObstacle(void)                                    {};
        aiObstacle(const aiObstacle&&)                      {};

        aiObstacle* GetNext()                               { return this->Next; }

        virtual BOOL InAccident(void);
        virtual void Position(Vector3& a1)                  PURE;
        virtual float Speed(void)                           PURE;
        virtual float BreakThreshold(void);
        virtual BOOL Drivable(void);
        virtual int CurrentRoadIdx(aiPath** a1, const bool* a2, int* a3)    
                                                            PURE;
        virtual int CurrentRdVert(void)                     
                                                            PURE;
        virtual void PreAvoid(const Vector3& a1, const Vector3& a2, float a3, Vector3& a4, Vector3& a5)
                                                            PURE;
        virtual float IsBlockingTarget(const Vector3& a1, const Vector3& a2, float a3, float a4)  
                                                            PURE;

        //lua
        static void BindLua(LuaState L);
    };

    class aiObstacleLuaIterator : public CppFunctor
    {
    private:
        aiObstacle* obstacle;
    public:
        aiObstacleLuaIterator(aiObstacle* obstacle)
        {
            this->obstacle = obstacle;
        }

        virtual ~aiObstacleLuaIterator()
        {
        }

        virtual int run(lua_State* L) override
        {
            if (this->obstacle)
            {
                LuaState(L).push(this->obstacle);
                this->obstacle = this->obstacle->GetNext();
                return 1;
            }
            else
            {
                return 0;
            }
        }
    };

    class aiVehicle : public aiObstacle {
    private:
        __int16 Id;
        __int16 FID;
        aiVehicle* NextAtStopSign;
    public:
        aiVehicle(void)                                     {};
        ~aiVehicle()                       {};

        int GetId() const;

        void PreAvoid(const Vector3& a1, const Vector3& a2, float a3, Vector3& a4, Vector3& a5) override;
        float IsBlockingTarget(const Vector3& a1, const Vector3& a2, float a3, float a4) override;
        
        virtual void Update(void);
        virtual void Reset(void);
        virtual int Type(void)                              PURE;
        virtual Matrix34 & GetMatrix(void) const            PURE;
        virtual float FrontBumperDistance(void)             PURE;
        virtual float BackBumperDistance(void)              PURE;
        virtual float LSideDistance(void)                   PURE;
        virtual float RSideDistance(void)                   PURE;
        virtual int CurrentLane(void)                       PURE;
        virtual int CurrentRoadId(void)                     PURE;
        virtual void DrawId(void) const                     PURE;
        virtual void ReplayDebug(void);

        //lua
        static void BindLua(LuaState L);
    };

    // Lua initialization

}