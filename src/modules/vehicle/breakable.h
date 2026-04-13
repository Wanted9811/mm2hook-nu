#pragma once
#include <modules/vector.h>
#include <modules/level/inst.h>

namespace MM2
{
    // Forward declarations
    class vehBreakable;
    class vehBreakableMgr;

    // External declarations
    extern class modStatic;
    
    class vehBreakable 
    {
    public:
        bool isAttached; // still attached to the parent?
        int index; // index into the list of breakables
        Matrix34 matrix;
        lvlInstance::GeomTableEntry* model;
        int bangerDataIndex;
        int geometryIndex;
        lvlInstance* instance;
        vehBreakable *next;
    public:
        AGE_API vehBreakable(modStatic** a1, Matrix34* a2, int a3, int a4, int a5);
        AGE_API void Add(vehBreakable* breakable);

        static void BindLua(LuaState L);
    };

    class vehBreakableMgr 
    {
    public:
        static bool EnableReflections;
    private:
        vehBreakable *first; // linked-list
        float impactThreshold; // educated guess (default: 10000.0)
        Matrix34 *matrix; // origin?
        int variant; // always zero by default, used in Eject to set Banger variant

        // TODO: figure out what these do

        float lastOneShotEjectVelocity; // (default: 4.0)
        float field_14; // (default: 1.0)
        float ejectVelocity; // (default: 2.0)
        float field_1C; // (default: 1.0)
    public:
        int GetVariant();
        void SetVariant(int variant);
        float GetImpactThreshold();
        void SetImpactThreshold(float threshold);
        float GetLastOneShotEjectVelocity();
        void SetLastOneShotEjectVelocity(float velocity);
        float GetEjectVelocity();
        void SetEjectVelocity(float velocity);
    public:
        ANGEL_ALLOCATOR

        AGE_API vehBreakableMgr();
        AGE_API ~vehBreakableMgr();

        AGE_API bool Init(Matrix34* carMatrix);
        AGE_API void Add(LPCSTR basename, LPCSTR breakableName, modStatic** model, int geomId, int index);
        AGE_API void Create(LPCSTR basename, LPCSTR breakableName, modStatic** model, int geomId, int index);
        AGE_API void Eject(vehBreakable* breakable, int room);
        AGE_API void EjectAll(int room);
        AGE_API void Reset();
        AGE_API void Draw(const Matrix34& world, modShader* shaders, int lod);
        AGE_API vehBreakable* Get(int index);

        AGE_API void Draw(const Matrix34& world, modShader* shaders, int lod, bool alphaState);
        AGE_API void DrawReflected(const Matrix34& world, modShader* shaders, int lod, int room);
        AGE_API void DrawShadowed(const Matrix34& world, modShader* shaders, float intensity);

        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(vehBreakable, 0x4C);
    ASSERT_SIZEOF(vehBreakableMgr, 0x20);
}