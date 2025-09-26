#pragma once
#include <modules\level\inst.h>
#include <modules\banger\data.h>

namespace MM2
{
    // Forward declarations
    class dgBangerInstance;

    class dgUnhitBangerInstance;
    class dgHitBangerInstance;

    class dgUnhitYBangerInstance;
    class dgUnhitMtxBangerInstance;

    class dgBangerManager;

    // External declarations
    extern class lvlInstance;

    // Class definitions
    
    /*
        Base Classes
    */

    // Note
    // The 16 byte gaps are most likely in the correct position
    // as suggested by dgUnhitBangerInstance::SizeOf in TDOR

    class dgBangerInstance : public lvlInstance {
    private:
        short DataPack; // Contains type and variant
    public:
        /*
            dgBangerInstance
        */

        dgBangerData* GetData() const;

        int GetVariant() const;

        void SetBangerType(unsigned short type);

        unsigned short GetBangerType() const;

        /*
            lvlInstance virtuals
        */

        virtual AGE_API void SetVariant(int a1)       override;
        virtual AGE_API dgPhysEntity* GetEntity()     override;
        virtual AGE_API dgPhysEntity* AttachEntity()  override;
        virtual AGE_API Vector3 const & GetVelocity() override;
        virtual AGE_API void Draw(int a1)             override;
        virtual AGE_API void DrawShadow()             override;
        virtual AGE_API void DrawShadowMap()          override;
        virtual AGE_API void DrawGlow()               override;
        virtual AGE_API void DrawReflected(float a1)  override;
        virtual AGE_API phBound* GetBound(int a1)     override;

        /* 
            dgBangerInstance virtuals
        */

        virtual AGE_API int ComputeLod(float a1, float a2);
        virtual AGE_API int SetupGfxLights(Matrix34 const & a1) const;
        virtual AGE_API void Impact(lvlInstance* a1, Vector3* a2)  PURE;
        virtual AGE_API void ImpactCB(dgHitBangerInstance * a1)    PURE;

                                                     
        static void BindLua(LuaState L);
    };

    class dgUnhitBangerInstance : public dgBangerInstance {
    private:
        static dgUnhitBangerInstance* requestBangerLua(LPCSTR name, bool fullMatrix);
    public:
        AGE_API dgUnhitBangerInstance();
        AGE_API ~dgUnhitBangerInstance();

        /*
            dgUnhitBangerInstance
        */

        AGE_API static dgUnhitBangerInstance* RequestBanger(const char* name, BOOL fullMatrix);

        /*
            lvlInstance virtuals
        */

        virtual AGE_API int Init(const char *a1, const Matrix34 &a2, int a3) override;
        virtual AGE_API void Reset() override;

        /*
            dgBangerInstance virtuals
        */

        virtual AGE_API void Impact(lvlInstance* a1, Vector3* a2);
        virtual AGE_API void ImpactCB(dgHitBangerInstance* a1);

        static void BindLua(LuaState L);
    };

    class dgHitBangerInstance : public dgBangerInstance {
    private:
        char gap[0x10]; // unknown use
        Matrix34 Matrix;
    public:
        AGE_API dgHitBangerInstance();
        AGE_API ~dgHitBangerInstance();

        /*
            lvlInstance virtuals
        */

        virtual AGE_API const Vector3& GetPosition() override;
        virtual AGE_API const Matrix34& GetMatrix(Matrix34* a1);
        virtual AGE_API void SetMatrix(const Matrix34& a1) override;
        virtual AGE_API unsigned int SizeOf(void);
        virtual AGE_API void Detach() override;

        static void BindLua(LuaState L);

    };
    ASSERT_SIZEOF(dgHitBangerInstance, 0x58);

    /*
        Derived
    */

    class dgUnhitYBangerInstance : public dgUnhitBangerInstance {
    private:
        char gap[0x10]; // unknown use
        Vector3 Position;
        float XAxis;
        float ZAxis;
    public:
        /*
            lvlInstance virtuals
        */

        virtual AGE_API const Vector3& GetPosition() override;
        virtual AGE_API const Matrix34& GetMatrix(Matrix34& a1);
        virtual AGE_API void SetMatrix(const Matrix34& a1) override;
        virtual AGE_API unsigned int SizeOf(void);

        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(dgUnhitYBangerInstance, 0x3C);


    class dgUnhitMtxBangerInstance : public dgUnhitBangerInstance {
    private:
        char gap[0x10]; // unknown use
        Matrix34 Matrix;
    public:
        /*
            lvlInstance virtuals
        */

        virtual AGE_API const Vector3& GetPosition() override;
        virtual AGE_API const Matrix34& GetMatrix(Matrix34& a1);
        virtual AGE_API void SetMatrix(const Matrix34& a1) override;
        virtual AGE_API unsigned int SizeOf(void);

        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(dgUnhitMtxBangerInstance, 0x58);

    /* 
        Manager
    */

    class dgBangerManager : public asNode {
    private:
        dgHitBangerInstance* HitBangerPool;
        int CurPoolIndex;
        int PoolSize;
    protected:
        static hook::Type<dgBangerManager*> Instance;
    public:
        static dgBangerManager * GetInstance();

        /*
            asNode virtuals
        */

        virtual AGE_API void Reset() override;

        /*
            dgBangerManager
        */

        AGE_API dgHitBangerInstance * GetBanger();
        AGE_API void Init(int poolSize);

        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(dgBangerManager, 0x24);
}