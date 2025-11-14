#pragma once
#include <modules\node\node.h>
#include <modules\banger\banger.h>

namespace MM2
{
    // Forward declarations
    class aiTrafficLightSet;
    class aiTrafficLightInstance;

    // External declarations
    extern class aiIntersection;

    // Class definitions
    class aiTrafficLightSet : public asNode {
    private:
        aiTrafficLightInstance** TrafficLights;
        aiIntersection* Intersection;
        __int16 IntersectionID;
        __int16 CurrentLightIdx;
        __int16 LightCount;
        __int16 Type;
        __int16 State;
        float CycleTime;
        float Timer;
    public:
        void SetCycleTime(float time);
        float GetCycleTime() const;
        float GetTimeInCycle() const;
        int GetCurrentLightIdx() const;
        int GetLightCount() const;

        aiTrafficLightInstance* GetLight(int num);

        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(aiTrafficLightSet, 0x34);

    class aiTrafficLightInstance : public dgUnhitYBangerInstance {
    private:
        __int16 State;
    private:
        int InitLua(LPCSTR name, Vector3 const& position, Vector3 const& positionFacing);
    public:
        aiTrafficLightInstance::aiTrafficLightInstance();

        /*
            lvlInstance virtuals
        */

        virtual AGE_API void Draw(int lod)            override;
        virtual AGE_API void DrawShadow()             override;
        virtual AGE_API void DrawGlow()               override;
        virtual AGE_API unsigned int SizeOf(void)     override;

        /* 
            aiTrafficLight virtuals
        */

        virtual AGE_API int Init(LPCSTR name, Vector3 const & position, Vector3 const & positionFacing, int unused_a5, LPCSTR unused_a6);

        int GetState() const;

        void SetState(int state);

        void DrawGlowShadow();

        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(aiTrafficLightInstance, 0x40);
}