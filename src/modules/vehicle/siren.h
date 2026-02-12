#pragma once
#include <mm2_common.h>
#include <modules\lights\light.h>
#include <modules\lights\flare.h>

namespace MM2
{
    // Forward declarations

    // External declarations
    extern class ltLight;

    // Class definitions
    class vehSiren
    {
    private:
        static const int MAX_LIGHTS = 8;
    public:
        static float SirenRotationSpeed;
    private:
        bool HasLights;
        bool Active;
        int LightCount;
        ltLight* ltLightPool;
        Vector3 lightPositions[4]; //we didn't use this field because it conflicts with the extra light positions which doesn't make them read their mtx files properly.
        float RotationRate;
        ltLensFlare* LensFlare;
        //EXTRA FIELD. The hook expands on this class, this is only possible because it's only used like a pointer in the original MM code
        Vector3 extraLightPositions[vehSiren::MAX_LIGHTS]; //SRN0-7
        bool enabledElectrics[vehSiren::MAX_LIGHTS]; //SRN0-7
    public:
        bool IsActive() const;
        void SetActive(bool active);
        bool GetHasLights() const;
        int GetLightCount() const;

        ltLight* GetLight(int index) const;

        Vector3 GetLightPosition(int index) const;

        void SetLightPosition(int index, Vector3 position);

        void SetEnabledElectrics(int index, bool state);

        //member funcs
        AGE_API bool Init();

        void RemoveAllLights();

        AGE_API bool AddLight(Vector3 const & position, Vector3 const & color);

        AGE_API void Reset();

        AGE_API void Update();

        AGE_API void Draw(Matrix34 const& mtx);

        static void BindLua(LuaState L);
    };

}