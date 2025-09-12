#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehWheelCheap;

    // External declarations
    extern class aiVehicleData;

    // Class definitions
    class vehWheelCheap : asNode {
    private:
        phInertialCS* ICS;
        float Spring;
        float Damping;
        lvlSegment Segment;
        lvlIntersection Intersection;
        lvlSegmentInfo* SegmentInfo;
        int dword_ec;
        int dword_f0;
        BOOL Grounded;
        Vector3 Position;
        float Radius;
        float QuarterGravity;
        Vector3 FricDirection;
        float RubberSpring;
        float RubberDamp;
        float Limit;
        BOOL ZeroDistance;
        Matrix34 Matrix;
        Matrix34 WorldMatrix;
    public:
        Vector3 GetPosition() const;
        Matrix34 GetMatrix() const;
        Matrix34 GetWorldMatrix() const;

        AGE_API vehWheelCheap();
        AGE_API ~vehWheelCheap();
        AGE_API void Init(Vector3* wheelPosition, aiVehicleData* data, phInertialCS* ICS);

        /*
            asNode virtuals
        */

        AGE_API void Update() override;
        AGE_API void Reset() override;

        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(vehWheelCheap, 0x188);
}