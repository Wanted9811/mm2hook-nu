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
    public:
        static float SirenRotationSpeed;
    private:
        bool HasLights;
        bool Active;
        int LightCount;
        ltLight* Light;
		Vector3* LightPositions; // This used to be an array of 4 positions, but the hook expands it to be as big as needed
        float RotationRate;
        ltLensFlare* LensFlare;
        //EXTRA FIELD. The hook expands on this class, this is only possible because it's only used like a pointer in the original MM code
        bool *LightElectrics;
		bool CustomGlowState;
    public:
		ANGEL_ALLOCATOR

		AGE_API vehSiren();
		AGE_API ~vehSiren();

        bool IsActive() const;
        void SetActive(bool active);
        bool GetHasLights() const;
        int GetLightCount() const;
        ltLight* GetLight(int index) const;
        Vector3 GetLightPosition(int index) const;
        void SetLightPosition(int index, Vector3& position);
        bool GetLightElectric(int index);
        void SetLightElectric(int index, bool state);
        bool GetCustomGlowState() const;
		void SetCustomGlowState(bool state);

        //member funcs
        AGE_API void Init(int lightCount);
        void RemoveAllLights();
        AGE_API void AddLight(Vector3 const& position, Vector3 const& color);
        AGE_API void Reset();
        AGE_API void Update();
        AGE_API void Draw(Matrix34 const& mtx);

        static void BindLua(LuaState L);
    };

}