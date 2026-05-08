#pragma once
#include <modules\level\inst.h>
#include <modules\effects\texeldamage.h>
#include <modules\effects\damage3d.h>
#include <modules\effects\damage.h>
#include <modules\effects\lightdamage.h>

#include "car.h"
#include "breakable.h"
#include "siren.h"
#include "cardamage.h"
#include "bound.h"

namespace MM2
{
    // Forward declarations
    class vehCarModel;

    // External declarations
    extern class dgPhysEntity;
    extern class lvlInstance;
    extern struct fxTexelDamage;
    extern class vehSiren;
    extern class vehCarDamage;

    // Class definitions

    class vehCarModel : public lvlInstance {
    public:
        static bool EnableSpinningWheels;
        static bool EnableHeadlightFlashing;
        static bool EnableWheelWobble;
        static bool EnableLightDamage;
        static bool MWStyleTotaledCar;
        static int Enable3DShadows;
        static int SirenType;
        static int HeadlightType;
        static float SirenCycle;
        static float HeadlightFlashingSpeed;
        static bool PartReflections;
        static bool WheelReflections;
        static bool DamageReflections;
        static bool MM1StyleTransmission;
        static bool MM1StyleDamage;
        static bool MM1StyleWobble;

        //light states
        static bool ShowHeadlights;
        static bool HazardLightsState;
        static bool LeftSignalLightState;
        static bool RightSignalLightState;

        //3d damage feature
        static bool Enable3DDamage;
        static bool Enable3DDynamicDamage;
    private:
        bool lightElectrics[4]; //0/1 are back, 2/3 are front
        vehCar* car;
        vehCarSim* carSim;
        fxTexelDamage* texelDamage;
        byte variant;
        byte char_25;
        byte char_26;
        byte char_27;
        int dword_28;
        float dummyWheelOffset;
        Vector3 fndr0offset;
        Vector3 fndr1offset;
        Vector3 trailerHitchPosition;
        int dword_54;
        int dword_58;
        int dword_5c;
        int dword_60;
        int dword_64;
        int dword_68;
        int dword_6c;
        int dword_70;
        int dword_74;
        int dword_78;
        int dword_7c;
        int dword_80;
        int dword_84;
        int dword_88;
        int dword_8c;
        Vector3 weirdOffsetPos;
        int dword_9c;
        vehBreakableMgr* genBreakableMgr;
        bool hasEjectedOneshot;
        char pad[3];
        int wheelBrokenStatus;
        vehBreakableMgr* wheelBreakableMgr;
        ltLight* headlights;
		Vector3* headlightPositions; // This used to be an array of 2 positions, but the hook expands it to be as big as needed
        //EXTRA FIELDS. The hook expands on this class, this is only possible because it's only used like a pointer in the original MM code
        bool* headlightElectrics;
        Vector3 fndr2offset;
        Vector3 fndr3offset;
        Vector3 fndr4offset;
        Vector3 fndr5offset;
        fxDamage3D* damage3D;
        mmDamage* mm1Damage;
        mmLightDamage* lightDamage;
        bool prevSplashState;
        bool customGlowState;
    private:
        Vector3 getSurfaceColorLua(modStatic* model);
    public:
        AGE_API vehCarModel();
        AGE_API ~vehCarModel();

        vehBreakableMgr* GetGenBreakableMgr();
        vehBreakableMgr* GetMechBreakableMgr();
        vehCar* GetCar();
        int GetVariant() const;
        ltLight* GetHeadlight(int index);
        Vector3 GetHeadlightPosition(int index);
		bool GetHeadlightElectric(int index);
        int GetWheelBrokenStatus() const;
        Vector3 GetTrailerHitchOffset() const;
        fxTexelDamage* GetTexelDamage();
        fxDamage3D* GetDamage3D();
        mmDamage* GetMM1Damage();
        mmLightDamage* GetLightDamage();
        bool GetPrevSplashState() const;
        void SetPrevSplashState(bool state);
        bool GetCustomGlowState() const;
        void SetCustomGlowState(bool state);

        Matrix34 GetWheelMatrix(int num);
        Matrix34 GetFenderMatrix(int num);
        Matrix34 GetWheelShadowMatrix(int num, const Matrix34& shadowMatrix);
        Matrix34 GetFenderShadowMatrix(int num, const Matrix34& shadowMatrix);
        float GetWobbleAngle(vehWheel* wheel, float wobbleMultiplier);
        Matrix34 GetBodyWobbleMatrix(const Matrix34& matrix, vehWheel* wheel, float wobbleMultiplier = 0.02f);
        Matrix34 GetPartWobbleMatrix(const Matrix34& matrix, vehWheel* wheel, float wobbleMultiplier = 0.02f);
        Matrix34 GetWorldMatrix();

        AGE_API void GetSurfaceColor(modStatic* model, Vector3& outColor);
        AGE_API void InitBreakable(vehBreakableMgr* manager, const char* basename, const char* breakableName, int geomId, int index);
        void InitBreakable(vehBreakableMgr* manager, const char* basename, const char* breakableName, int index);
        AGE_API void InitSirenLight(const char* basename, const char* mtxName, int geomId);
        void InitSirenLights(const char* basename);
        void InitHeadlights(const char* basename);
        AGE_API void BreakElectrics(const Vector3& localImpactPos);
        AGE_API void ClearDamage();
        AGE_API void EjectOneshot();
        AGE_API bool GetVisible();
        AGE_API void SetVisible(bool visible);
        AGE_API void DrawHeadlights(bool rotate);
        AGE_API void DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders);
        AGE_API void DrawPart(int lod, int geomId, const Matrix34& matrix, modShader* shaders);
        void DrawPartReflected(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity, bool reflected = true);
        void DrawPartShadowed(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity);
        void Init(vehCar* car, const char* basename, int variant);
        
        /*
            lvlInstance virtuals
        */

        AGE_API void Reset() override;
        AGE_API const Vector3& GetPosition() override;
        AGE_API const Matrix34& GetMatrix(Matrix34& a1) override;
        AGE_API void SetMatrix(const Matrix34& a1) override;
        AGE_API void SetVariant(int variant) override;
        AGE_API dgPhysEntity* GetEntity() override;
        AGE_API dgPhysEntity* AttachEntity() override;
        AGE_API const Vector3& GetVelocity() override;
        AGE_API void Draw(int lod) override;
        AGE_API void DrawShadow() override;
        AGE_API void DrawShadowMap() override;
        AGE_API void DrawGlow() override;
        AGE_API void DrawReflected(float intensity) override;
        AGE_API void DrawReflectedParts(int lod) override;
        AGE_API unsigned int SizeOf() override;

        static void BindLua(LuaState L);
    };
}