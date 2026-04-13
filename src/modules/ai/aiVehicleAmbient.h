#pragma once
#include "aiVehicleSpline.h"
#include <modules\rgl.h>

namespace MM2
{
    // Forward declarations
    class aiVehicleAmbient;

    // External declarations


    // Class definitions

    class aiVehicleAmbient : public aiVehicleSpline {
    private:
        byte _buffer[0x14];
    public:
        AGE_API aiVehicleAmbient();
        AGE_API ~aiVehicleAmbient();

        AGE_API void Update() override;
        AGE_API void Reset() override;
        AGE_API int Type() override;
        AGE_API void DrawId() const override;
        AGE_API void ReplayDebug() override;
        AGE_API void Impact(int a1) override;

        //members
        AGE_API void Init(const char* name, int id);
        AGE_API void Dump();
        AGE_API void DrawBBox(bool selected);

        //lua
        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(aiVehicleAmbient, 0x1A0);

    // Lua initialization

}