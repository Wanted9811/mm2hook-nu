#pragma once
#include <modules\banger.h>
#include <modules\effects\birth.h>

namespace MM2
{
    // Forward declarations
    class dgBangerData;
    class dgBangerDataManager;

    // External declarations
    extern class asNode;
    extern class asBirthRule;
    extern class dgBangerGlowData;

    // Class definitions

    class dgBangerData : public asNode
    {
    public:
        uint Index;
        Vector3 CG;
        Vector3 Size;
        int dword34;
        int dword38;
        int dword3C;
        dgBangerGlowData* GlowDatas; // This field was Vector3* GlowOffsets originally but we hooked it to add more glow data, this is only possible since it's a pointer
        short NumGlows;
        short word46;
        float Mass;
        float Elasticity;
        float Friction;
        float ImpulseLimit2;
        float YRadius;
        int ColliderId;
        asBirthRule BirthRule;
        int TexNumber;
        int BillFlags;
        short SpinAxis;
        short Flash;
        byte CollisionType;
        byte CollisionPrim;
        short unknown290;
        phBound* Bound;
        short NumParts;
        short unknown298;
        int unknown300;
        int AudioId;
        int GeomSet;
        char pad138[28];
    public:
        Vector3 GetGlowOffset(int num) const;

        void SetGlowOffset(int num, Vector3 offset);
    public:
        void InitBound();

        /*
            asNode virtuals
        */

        AGE_API bool Save() override;
        AGE_API bool Load() override;
        AGE_API char* GetClassName() override;
        AGE_API const char* GetDirName() override;

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(dgBangerData, 0x154);

    /*
        dgBangerDataManager
    */

    class dgBangerDataManager : public asNode 
    {
    private:
        const static int NUM_BANGER_TYPES = 512;
    private:
        int dataCount;
        dgBangerData datas[NUM_BANGER_TYPES];
        gfxTexture* ParticleSheetTextures[20];
        byte buffer[0x58];
    public:
        int GetDataCount() const;

        dgBangerData* GetData(int id);
    protected:
        static hook::Type<dgBangerDataManager*> Instance;
    public:
        static dgBangerDataManager* GetInstance();
    
        /*
            asNode virtuals
        */

        AGE_API char* GetClassName() override;

        /*
            dgBangerDataManager
        */

        AGE_API int AddBangerDataEntry(char const* name, char const* partName);

        AGE_API void ChangeData();

        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(dgBangerDataManager, 0x2A8C4);

    // Lua initialization

}