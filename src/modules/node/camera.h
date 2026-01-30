#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward declarations
    class camBaseCS;
    class camAppCS;
    class camCarCS;
    class camPointCS;
    class camPovCS;
    class camTrackCS;
    class camPolarCS;
    class camAICS;
    class camViewCS;
    class asCamera;

    // External declarations
    extern class asNode;
    extern class vehCar;

    // Class definitions
    struct Spline : public asNode
    {
        /* This isn't really used at all, but it's a member of camTrackCS */
    public:
        int npoints;
        float* a;
        float* b;
        float* c;
        float* d;
        float* cc0;
        float* cc1;
        float* cc2;
        float* cc3;
        float* v;
        float* dword_40;
        float* dword_44;
        float t0;
        float t1;
        float t2;
        float t3;
        float dword_58;
        float dword_5c;
        float timeEnd;
        float timeStart;
        int dword_68;
    };
    ASSERT_SIZEOF(Spline, 0x6C);

    class asCamera : public asNode {
    private:
        byte _buffer[0x158];
    public:
        AGE_API asCamera(void);

        virtual AGE_API ~asCamera(void);

        AGE_API void SetView(float fov, float aspect, float nearClip, float farClip);
        AGE_API void SetViewport(float left, float top, float width, float height);
        AGE_API void Update() override;

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(asCamera, 0x170);

    class camBaseCS : public asNode {
    protected:
        uint16_t Flags;
        uint16_t Padding; // unused
        Matrix34 WorldMatrix;
        Matrix34 Matrix;
        camViewCS *View;
        float BlendTime;
        float BlendGoal;
        float CameraFOV;
        float CameraNear;
    private:
        static hook::Type<float> sm_CameraFar;
    public:
        AGE_API camBaseCS(void);

        virtual AGE_API ~camBaseCS(void);

        //fields
        Matrix34 GetMatrix() const;

        void SetMatrix(Matrix34 matrix);
        
        Vector3 GetPosition() const;

        void SetPosition(Vector3 position);

        float GetFOV() const;
        
        void SetFOV(float fov);

        void SetCamView(camViewCS* view);

        static float GetCameraFar();

        static void SetCameraFar(float value);

        //asNode overrides
        AGE_API void AfterLoad() override;
        AGE_API void FileIO(datParser& parser) override;
        AGE_API const char * GetDirName() override;

        //virtuals
        virtual AGE_API void MakeActive();
        virtual AGE_API void UpdateInput();
        virtual AGE_API void ForceMatrixDelta(const Vector3 &a1);
        virtual AGE_API void SetST(float *a1);

        //members
        AGE_API BOOL IsViewCSInTransition();
        AGE_API void UpdateView();

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(camBaseCS, 0x90);

    class camAppCS : public camBaseCS {
    protected:
        Matrix34 *Target;
        Vector3 TrackTo;
        BOOL ApproachOn;
        BOOL AppAppOn;
        float AppRot;
        float AppXRot;
        float AppYPos;
        float AppXZPos;
        float AppApp;
        float AppRotMin;
        float AppPosMin;
        float LookAbove;
        int OneShot;
        float MaxDist;
        float MinDist;
        float LookAt;
        Matrix34 unk_D8; // approach stuff
    protected:
        AGE_API void ApproachIt();

    public:
        AGE_API camAppCS(void);

        virtual AGE_API ~camAppCS(void);

        void SetApproachOn(bool value);
        bool GetApproachOn() const;
        void SetAppAppOn(bool value);
        bool GetAppAppOn() const;

        Matrix34* GetTarget();

        //overrides
        AGE_API void FileIO(datParser &parser) override;
    
        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(camAppCS, 0x108);

    class camCarCS : public camAppCS {
    protected:
        vehCar *Car;
        int ReverseMode; // one of: -1, 0, 1?
    public:
        vehCar * GetCar() const;

        void SetCar(vehCar* car);

        int GetReverseMode();

        AGE_API camCarCS(void);

        virtual AGE_API ~camCarCS(void);

        AGE_API void Init(vehCar *car, LPCSTR name);

        //overrides
        AGE_API void FileIO(datParser &parser) override;

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(camCarCS, 0x110);

    class camPointCS : public camCarCS {
    protected:
        Vector3 Position;
        Vector3 Velocity;
        float MaxDistSqr;
        float MaxDist;
        float AppRate;
        float MinDist;
        float MinDistSqr;
    public:
        AGE_API camPointCS(void);

        virtual AGE_API ~camPointCS(void);

        float GetMinDist() const;

        float GetMaxDist() const;

        float GetAppRate()  const;

        AGE_API void SetAppRate(float rate);
        AGE_API void SetMaxDist(float dist);
        AGE_API void SetMinDist(float dist);
        AGE_API void SetPos(Vector3 const &pos);
        AGE_API void SetVel(Vector3 const &vel);

        //camBaseCS overrides
        AGE_API void MakeActive() override;

        //asNode overrides
        AGE_API void Reset() override;
        AGE_API void Update() override;
        AGE_API char * GetClassName() override;

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(camPointCS, 0x13C);

    class camPovCS : public camCarCS {
    protected:
        int unk_110;
        Vector3 Offset;
        Vector3 ReverseOffset;
        float POVJitterAmp;
        Vector3 unk_130;
        float Angle; // related to camCarCS::ReverseMode: if -1, use PI; otherwise, use this value (see: camPOVCS::Update @ 0x51D62A)
        float Pitch;
        float Yaw; // rotates matrix if != 0
    public:
        AGE_API void UpdatePOV();
    public:
        Vector3 GetOffset() const;

        Vector3 GetReverseOffset() const;

        float GetAngle() const;

        float GetPitch() const;

        float GetYaw() const;

        AGE_API camPovCS(void);

        virtual AGE_API ~camPovCS(void);

        //camBaseCS overrides
        AGE_API void MakeActive() override;
        AGE_API void UpdateInput() override;

        //asNode overrides
        AGE_API void Reset() override;
        AGE_API void Update() override;
        AGE_API void FileIO(datParser &parser) override;
        AGE_API void AfterLoad() override;
        AGE_API char * GetClassName() override;

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(camPovCS, 0x148);

    class camTrackCS : public camCarCS {
    protected:
        int dword_110;
        Vector3 Offset;
        int CollideType; //0, 1, or 2
        BOOL MinMaxOn;
        int TrackBreak; //0, 1, or 2
        float MinAppXZPos;
        float MaxAppXZPos;
        float MinSpeed;
        float MaxSpeed;
        float AppInc;
        float AppDec;
        float MinHardSteer;
        float DriftDelay;
        float VertOffset;
        float FrontRate;
        float RearRate;
        float FlipDelay;
        BOOL SteerOn;
        float SteerMin;
        float SteerAmt;
        float HillMin;
        float HillMax;
        float HillLerp;
        float RevDelay;
        float RevOnApp;
        float RevOffApp;
        byte _buffer1[0x3C];
        Spline m_Spline;
        byte _buffer2[0x68];
        int CollideFlags1;
        int CollideFlags2;
    private:
        AGE_API void Collide(Vector3 a1);
        AGE_API void Front(float a1);
        AGE_API void MinMax(Matrix34 a1);
        AGE_API void PreApproach();
        AGE_API void Rear(float a1);

        AGE_API void UpdateCar();
        AGE_API void UpdateHill();
        AGE_API void UpdateTrack();
    public:
        AGE_API camTrackCS(void);

        virtual AGE_API ~camTrackCS(void);

        AGE_API void SwingToRear();

        bool GetSteerOn() const;
        void SetSteerOn(bool value);
        bool GetMinMaxOn() const;
        void SetMinMaxOn(bool value);

        //asNode overrides
        AGE_API void AfterLoad() override;
        AGE_API void FileIO(datParser& parser) override;
        AGE_API char * GetClassName() override;
        AGE_API void Reset() override;
        AGE_API void Update() override;

        //camBaseCS overrides
        AGE_API void MakeActive() override;
        AGE_API void UpdateInput() override;

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(camTrackCS, 0x298);

    class camPolarCS : camCarCS {
    private:
        float PolarHeight;
        float PolarDistance;
        float PolarAzimuth;
        float PolarIncline;
        float PolarDelta;
        BOOL AzimuthLock;
    public:
        AGE_API camPolarCS(void);

        virtual AGE_API ~camPolarCS(void);

        bool GetAzimuthLock();

        void SetAzimuthLock(bool lock);

        //asNode overrides
        AGE_API void FileIO(datParser &parser) override;
        AGE_API char * GetClassName() override;
        AGE_API void Reset() override;
        AGE_API void Update() override;

        //camBaseCS overrides
        AGE_API void MakeActive() override;

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(camPolarCS, 0x128);

    class camAICS : public camCarCS {
    private:
        float Speed;
        float RotationSpeed;
        byte SegmentInfo[12]; // lvlSegmentInfo
    public:
        AGE_API camAICS(void);

        virtual AGE_API ~camAICS(void);

        static float getHeight(void);

        static void setHeight(float height);

        AGE_API void SetPosition(Vector3 *a1);
        AGE_API void SetMatrix(Matrix34 *a1);

        // asNode overrides
        AGE_API void Reset() override;

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(camAICS, 0x124);

    class camTransitionCS : public camCarCS {
    private:
        camCarCS* FromCamera;
        camCarCS* ToCamera;
        camCarCS* dword_118;
        int dword_11c;
        int dword_120;
        int dword_124;
    public:
        AGE_API camTransitionCS(void);

        virtual AGE_API ~camTransitionCS(void);

        // asNode overrides
        AGE_API void Reset() override;
        AGE_API void Update() override;

        //lua
        static void BindLua(LuaState L);
    };
    ASSERT_SIZEOF(camTransitionCS, 0x128);

    class camViewCS : public asNode {
    protected:
        BOOL ManualFOV;
        short LastTransitionType;
        float LastTransitionTime;
        datCallback Callback;
        camCarCS *CurrentCamera;
        camCarCS *AfterTransitionCamera;
        camTransitionCS *TransitionCamera;
        bool UseGlobalNearFar; 
        float FarClip;
        float NearClip;
        int unk_48; // unused
    private:
        //helpers for lua calls
        bool NewCam(camCarCS* cam, int type, float time);
    public:
        AGE_API camViewCS(void);

        virtual AGE_API ~camViewCS(void);

        camCarCS * GetCurrentCamera(void) const;
        bool IsCurrentCamera(camCarCS *cam) const;
        bool IsTransitioning() const;

        AGE_API static camViewCS * Instance(vehCar *a1);

        AGE_API void SetCam(camCarCS *cam);
        AGE_API void OneShot();

        AGE_API BOOL NewCam(camCarCS *cam, int type, float time, datCallback postTransitionCB);

        AGE_API void Init(vehCar *car);

        AGE_API void ForceMatrixDelta(const Vector3 *a1);
        AGE_API void ForceMatrixDelta(const Matrix34 *a1);

        //overrides
        AGE_API void Update() override;
        AGE_API void Reset() override;
        AGE_API void FileIO(datParser &parser) override;

        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(camViewCS, 0x4C);

    // Lua initialization
    template<>
    void luaAddModule<module_camera>(LuaState L) {
        //cam base classes
        luaBind<asCamera>(L);
        luaBind<camBaseCS>(L);
        luaBind<camAppCS>(L);
        luaBind<camCarCS>(L);

        //camera types
        luaBind<camPointCS>(L);
        luaBind<camPovCS>(L);
        luaBind<camPolarCS>(L);
        luaBind<camAICS>(L);
        luaBind<camTrackCS>(L);

        //view
        luaBind<camViewCS>(L);
    }
}