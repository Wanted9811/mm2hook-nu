#pragma once
#include <modules\ai\aiVehicleData.h>
#include <modules\vehicle\wheelcheap.h>

namespace MM2
{
    // Forward declarations
    class aiVehicleActive;
    class aiVehicleManager;

    // External declarations
    extern class asNode;
    extern class ltLight;

    // Class definitions
    class aiVehicleActive : dgPhysEntity
    {
    private:
        datCallback* Callback;
        asParticles Particles;
        asParticles Particles2;
        asBirthRule BirthRule;
        asBirthRule BirthRule2;
        float CurDamage;
        float MaxDamage;
        int NumActives;
        aiVehicleInstance* VehicleInstance;
        phInertialCS ICS;
        phSleep Sleep;
        vehWheelCheap WHL0;
        vehWheelCheap WHL1;
        vehWheelCheap WHL2;
        vehWheelCheap WHL3;
    public:

        vehWheelCheap* GetWheel(int num)
        {
            switch (num) {
            case 0:
                return &this->WHL0;
            case 1:
                return &this->WHL1;
            case 2:
                return &this->WHL2;
            case 3:
                return &this->WHL3;
            }
            return nullptr;
        }

        //dgPhysEntity overrides
        virtual AGE_API void Update() override                    { hook::Thunk<0x553890>::Call<void>(this); }
        virtual AGE_API void PostUpdate() override                { hook::Thunk<0x553960>::Call<void>(this); }
        virtual AGE_API phInertialCS* GetICS() override           { return hook::Thunk<0x5543B0>::Call<phInertialCS*>(this); }
        virtual AGE_API lvlInstance* GetInst() override           { return this->VehicleInstance; }
        virtual AGE_API void DetachMe() override                  { hook::Thunk<0x553690>::Call<void>(this); }
        virtual AGE_API bool RequiresTerrainCollision() override  { return hook::Thunk<0x553A20>::Call<bool>(this); }

    };
    ASSERT_SIZEOF(aiVehicleActive, 0xAFC);

    class aiVehicleManager : public asNode {
    private:
        aiVehicleData vehicleDatas[32];
        int numVehicleDatas;
        aiVehicleActive* activeActives[32];
        aiVehicleActive aiVehicleActives[32];
        __int16 attachedCount;
        ltLight* sharedLight;
    public:
        static hook::Type<aiVehicleManager *> Instance;
        static hook::Type<int> SignalClock;

        AGE_API aiVehicleManager(void) {
            scoped_vtable x(this);
            hook::Thunk<0x553B30>::Call<void>(this);
        }

        AGE_API virtual ~aiVehicleManager(void) {
            scoped_vtable x(this);
            hook::Thunk<0x553C2>::Call<void>(this);
        }

        //members
        AGE_API void Init(char *unused)                           { hook::Thunk<0x553CE0>::Call<void>(this, unused); }
        AGE_API int AddVehicleDataEntry(LPCSTR name)              { return hook::Thunk<0x553FA0>::Call<int>(this, name); }
        AGE_API void SaveEntry()                                  { hook::Thunk<0x5541E0>::Call<void>(this); }
        /*
        AGE_API aiVehicleActive Attach
        AGE_API aiVehicleActive Detach
        */

        //asNode overrides
        AGE_API void Reset() override                             { hook::Thunk<0x553D60>::Call<void>(this); }
        AGE_API void Update() override                            { hook::Thunk<0x553EA0>::Call<void>(this); }

        //helpers
        int GetDataCount() {
            return this->numVehicleDatas;
        }

        aiVehicleData * GetData(int num) {
            if (num < 0 || num >= GetDataCount())
                return nullptr;
            return &this->vehicleDatas[num];
        }

        ltLight* GetSharedLight()
        {
            return this->sharedLight;
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleManager, asNode>("aiVehicleManager")
                .addStaticProperty("Instance", [] { return Instance.get(); })
                
                .addFunction("AddVehicleDataEntry", &AddVehicleDataEntry)
                .addFunction("SaveEntry", &SaveEntry)

                .addPropertyReadOnly("NumVehicleDatas", &GetDataCount)
                .addPropertyReadOnly("DataCount", &GetDataCount) // old naming convention
                .addFunction("GetData", &GetData)
            .endClass();
        }
    };

    ASSERT_SIZEOF(aiVehicleManager, 0x177A4);
}