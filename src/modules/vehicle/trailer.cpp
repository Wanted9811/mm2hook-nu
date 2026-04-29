#include "trailer.h"
#include <modules\city\citylevel.h>
#include <modules\mmcityinfo\state.h>
#include <modules\mmgame\gameman.h>

namespace MM2
{
    /*
        vehTrailer
    */

    const float BlurSpeed = 26.0f;

    phInertialCS* vehTrailer::GetInertialCS()
    {
        return &this->ICS;
    }

    vehCarSim* vehTrailer::GetCarSim()
    {
        return this->GetJoint()->GetCarSim();
    }

    dgTrailerJoint* vehTrailer::GetJoint()
    {
        return &this->TrailerJoint;
    }

    vehWheel * vehTrailer::GetWheel(int num)
    {
        switch (num) {
        case 0:
            return &this->TWHL0;
        case 1:
            return &this->TWHL1;
        case 2:
            return &this->TWHL2;
        case 3:
            return &this->TWHL3;
        }
        return nullptr;
    }

    lvlTrackManager* vehTrailer::GetWheelTrackManager(int num)
    {
        switch (num) {
        case 0:
            return &this->TWHL0TrackManager;
        case 1:
            return &this->TWHL1TrackManager;
        case 2:
            return &this->TWHL2TrackManager;
        case 3:
            return &this->TWHL3TrackManager;
        }
        return nullptr;
    }

    vehTrailerInstance* vehTrailer::GetInstance()
    {
        return &this->TrailerInstance;
    }

    Vector3 vehTrailer::GetCarHitchOffset() const
    {
        return this->CarHitchOffset;
    }

    Vector3 vehTrailer::GetTrailerHitchOffset() const
    {
        return this->TrailerHitchOffset;
    }

    AGE_API vehTrailer::vehTrailer()
    {
        hook::Thunk<0x4D6F40>::Call<void>(this);
    }

    AGE_API vehTrailer::~vehTrailer()
    {
        hook::Thunk<0x4D71A0>::Call<void>(this);
    }

    AGE_API void vehTrailer::Init(const char* basename, const Vector3& carHitchOffset, vehCarSim* carSim, int variant)
    {
        hook::Thunk<0x4D72F0>::Call<void>(this, basename, &carHitchOffset, carSim, variant);

        Matrix34 diffMatrix;

        if (GetPivot(diffMatrix, basename, "trailer_twhl4")) {
            GetCarSim()->TrailerBackBackLeftWheelPosDiff = diffMatrix.GetRow(3) - GetWheel(2)->GetCenter();
        }

        if (GetPivot(diffMatrix, basename, "trailer_twhl5")) {
            GetCarSim()->TrailerBackBackRightWheelPosDiff = diffMatrix.GetRow(3) - GetWheel(3)->GetCenter();
        }
    }

    /*
        asNode virtuals
    */

    AGE_API void vehTrailer::Reset()                        { hook::Thunk<0x4D79C0>::Call<void>(this); };
    AGE_API void vehTrailer::FileIO(datParser &parser)      { hook::Thunk<0x4D7CB0>::Call<void>(this, &parser); };
    AGE_API bool vehTrailer::Save()                         { return hook::Thunk<0x4D7D90>::Call<bool>(this); };
    AGE_API bool vehTrailer::Load()                         { return hook::Thunk<0x4D7DC0>::Call<bool>(this); };
    AGE_API char * vehTrailer::GetClassName()               { return hook::Thunk<0x4D8510>::Call<char *>(this); };
    AGE_API const char * vehTrailer::GetDirName()           { return hook::Thunk<0x4D72E0>::Call<const char *>(this); };

    /*
        dgPhysEntity virtuals
    */

    AGE_API void vehTrailer::Update()                       { hook::Thunk<0x4D7B00>::Call<void>(this); }
    AGE_API void vehTrailer::PostUpdate()                   { hook::Thunk<0x4D7C40>::Call<void>(this); }
    AGE_API phInertialCS * vehTrailer::GetICS()             { return hook::Thunk<0x4D8520>::Call<phInertialCS*>(this); }
    AGE_API lvlInstance * vehTrailer::GetInst()             { return hook::Thunk<0x4D7860>::Call<lvlInstance*>(this); }
    AGE_API bool vehTrailer::RequiresTerrainCollision()     { return hook::Thunk<0x4D7870>::Call<bool>(this); }

    void vehTrailer::BindLua(LuaState L) {
        LuaBinding(L).beginExtendClass<vehTrailer, dgPhysEntity>("vehTrailer")
            .addFunction("GetWheel", &GetWheel)
            .addPropertyReadOnly("CarSim", &GetCarSim)
            .addPropertyReadOnly("Joint", &GetJoint)
            .addPropertyReadOnly("Instance", &GetInstance)
            .addPropertyReadOnly("CarHitchOffset", &GetCarHitchOffset)
            .addPropertyReadOnly("TrailerHitchOffset", &GetTrailerHitchOffset)
        .endClass();
    }

    /*
        vehTrailerInstance
    */

    Vector3 vehTrailerInstance::getTrailerHitchOffsetLua() const
    {
        Vector3 vec;
        this->GetTrailerHitch(&vec);
        return vec;
    }

    vehTrailer* vehTrailerInstance::GetTrailer(void) const
    {
        return this->Trailer;
    }

    Matrix34 vehTrailerInstance::GetWheelMatrix(int num)
    {
        Matrix34 whlMatrix = Matrix34();

        if (num < 4)
        {
            return this->Trailer->GetWheel(num)->GetMatrix();
        }
        else
        {
            Vector3 whlPosDiff[2] = { this->Trailer->GetCarSim()->TrailerBackBackLeftWheelPosDiff, this->Trailer->GetCarSim()->TrailerBackBackRightWheelPosDiff };
            whlMatrix.Set(this->Trailer->GetWheel(num - 2)->GetMatrix());
            whlMatrix.SetRow(3, whlMatrix.GetRow(3) + this->Trailer->GetInertialCS()->GetMatrix().Transform3x3(whlPosDiff[num - 4]));
        }

        return whlMatrix;
    }

    Matrix34 vehTrailerInstance::GetWheelShadowMatrix(int num, const Matrix34& shadowMatrix)
    {
        Vector3 extraWhlPosDiff[2] = { this->Trailer->GetCarSim()->TrailerBackBackLeftWheelPosDiff, this->Trailer->GetCarSim()->TrailerBackBackRightWheelPosDiff };
        Matrix34 whlShadowMatrix = Matrix34();
        whlShadowMatrix.Identity();

        if (num < 4)
        {
            auto wheel = this->Trailer->GetWheel(num);
            whlShadowMatrix.SetRow(3, wheel->GetCenter());
            whlShadowMatrix.Rotate(whlShadowMatrix.GetRow(0), wheel->GetAccumulatedRotation());
            whlShadowMatrix.RotateY(wheel->GetSteerAmount());
            whlShadowMatrix.m31 += wheel->GetBumpDisplacement();
        }
        else
        {
            auto wheel = this->Trailer->GetWheel(num - 2);
            whlShadowMatrix.SetRow(3, wheel->GetCenter() + extraWhlPosDiff[num - 4]);
            whlShadowMatrix.Rotate(whlShadowMatrix.GetRow(0), wheel->GetAccumulatedRotation());
            whlShadowMatrix.RotateY(wheel->GetSteerAmount());
            whlShadowMatrix.m31 += wheel->GetBumpDisplacement();
        }

        whlShadowMatrix.Dot(shadowMatrix);
        return whlShadowMatrix;
    }

    void vehTrailerInstance::DrawPartReflected(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity, bool reflected)
    {
        if (reflected)
        {
            auto model = this->GetGeom(lod, geomId);
            if (model != nullptr)
            {
                auto reflectionMap = lvlLevel::GetSingleton()->GetEnvMap(this->GetRoomId(), this->GetPosition(), intensity);
                if (reflectionMap != nullptr)
                {
                    modShader::BeginEnvMap(reflectionMap, matrix);
                    model->DrawEnvMapped(shaders, reflectionMap, intensity);
                    modShader::EndEnvMap();
                }
            }
        }
    }

    void vehTrailerInstance::DrawPartShadowed(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity)
    {
        auto model = this->GetGeom(lod, geomId);
        if (model != nullptr)
        {
            gfxRenderState::SetWorldMatrix(matrix);
            model->DrawShadowed(shaders, intensity);
        }
    }

    void vehTrailerInstance::DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders)
    {
        gfxRenderState::SetWorldMatrix(matrix);
        model->Draw(shaders);
    }

    void vehTrailerInstance::DrawPart(int lod, int geomId, const Matrix34& matrix, modShader* shaders)
    {
        auto model = this->GetGeom(lod, geomId);
        if (model != nullptr)
            DrawPart(model, matrix, shaders);
    }

    AGE_API void vehTrailerInstance::Init(const char* basename, Vector3 const& unused, int variant)
    {
        this->SetFlag(0xC8);
        bool hasGeometry = false;

        if (lvlInstance::BeginGeom(basename, "trailer", 0xD))
        {
            lvlInstance::AddGeoms(basename, "trailer");
            lvlInstance::EndGeom();
            hasGeometry = true;
        }

        this->Variant = variant % this->GetVariantCount();
        lvlInstance::PreLoadShader(this->Variant);

        if (hasGeometry)
            lvlInstance::Optimize(this->Variant);

        auto trailerHitch = lvlInstance::GetGeom(3, this->GetGeomId("trailer_hitch"));
        if (trailerHitch != nullptr)
        {
            Matrix34 outMatrix;
            GetPivot(outMatrix, basename, "trailer_hitch");
            this->HitchOffset = outMatrix.GetRow(3);
        }
    }

    AGE_API const Vector3& vehTrailerInstance::GetPosition()
    {
        return hook::Thunk<0x4D7810>::Call<const Vector3&>(this);
    }

    AGE_API const Matrix34& vehTrailerInstance::GetMatrix(Matrix34& a1)
    {
        return hook::Thunk<0x4D77F0>::Call<const Matrix34&>(this, &a1);
    }

    AGE_API void vehTrailerInstance::SetMatrix(const Matrix34& a1)
    {
        hook::Thunk<0x4D77D0>::Call<void>(this, &a1);
    }

    AGE_API void vehTrailerInstance::Draw(int lod)
    {
        if (lod < 0 || lod > 3)
            return;
        if (this->GetGeomIndex() == 0)
            return;

        //get shaders
        auto shaders = this->GetShader(this->Variant);

        //setup renderer
        auto trailer = this->GetTrailer();
        auto trailerMatrix = trailer->GetInertialCS()->GetMatrix();
        gfxRenderState::SetWorldMatrix(trailerMatrix);

        //draw the trailer
        auto trailerGeom = this->GetGeom(lod, 0);
        if (trailerGeom != nullptr)
            trailerGeom->DrawNoGlass(shaders);

        //get wheels
        vehWheel* wheels[6] = { trailer->GetWheel(0), trailer->GetWheel(1), trailer->GetWheel(2), trailer->GetWheel(3), trailer->GetWheel(2), trailer->GetWheel(3) };

        //draw (s)whl0-5
        for (int i = 0; i < 6; i++)
        {
            string_buf<16> swhl("tswhl%d", i);
            string_buf<16> whl("twhl%d", i);
            int swhlId = this->GetGeomId(swhl);
            int whlId = this->GetGeomId(whl);
            auto wheel = wheels[i];
            auto swhlGeom = this->GetGeom(lod, swhlId);
            if (fabs(wheel->GetRotationRate()) > BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
            {
                DrawPart(lod, swhlId, this->GetWheelMatrix(i), shaders);
            }
            else
            {
                DrawPart(lod, whlId, this->GetWheelMatrix(i), shaders);
            }
        }
    }

    AGE_API void vehTrailerInstance::DrawShadow()
    {
        if (vehCarModel::Enable3DShadows <= 1
            || MMSTATE->TimeOfDay == 3
            || lvlLevel::GetSingleton()->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
        {
            // draw drop shadow
            hook::Thunk<0x4D81F0>::Call<void>(this);
            return;
        }

        auto timeWeather = cityLevel::GetCurrentLighting();

        bool prevLighting = gfxRenderState::SetLighting(true);

        //invert model faces
        auto prevCullMode = gfxRenderState::GetCullMode();
        gfxRenderState::SetCullMode(D3DCULL_CCW);

        //get trailer
        auto trailer = this->GetTrailer();

        //get shaders
        auto shaders = this->GetShader(this->Variant);

        //get model
        modStatic* model = this->GetGeomBase()->GetHighestLOD();

        if (model != nullptr)
        {
            Matrix34 shadowMatrix = Matrix34();
            Matrix34 trailerMatrix = trailer->GetInertialCS()->GetMatrix();

            if (lvlInstance::ComputeShadowProjectionMatrix(shadowMatrix, this->GetRoomId(), timeWeather->KeyPitch, timeWeather->KeyHeading, trailerMatrix, this))
            {
                gfxRenderState::SetWorldMatrix(shadowMatrix);
                float intensity = ComputeShadowIntensity(timeWeather->KeyColor);
                model->DrawShadowed(shaders, intensity);

                //get wheel ids
                vehWheel* wheels[6] = { trailer->GetWheel(0), trailer->GetWheel(1), trailer->GetWheel(2), trailer->GetWheel(3), trailer->GetWheel(2), trailer->GetWheel(3) };

                //draw wheels
                for (int i = 0; i < 6; i++)
                {
                    string_buf<16> whl("twhl%d", i);
                    int whlId = this->GetGeomId(whl);
                    auto whlGeom = this->GetGeom(3, whlId);
                    if (whlGeom != nullptr)
                    {
                        string_buf<16> swhl("tswhl%d", i);
                        int swhlId = this->GetGeomId(swhl);
                        auto swhlGeom = this->GetGeom(3, swhlId);
                        auto wheel = wheels[i];
                        if (fabs(wheel->GetRotationRate()) > BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
                        {
                            DrawPartShadowed(3, swhlId, this->GetWheelShadowMatrix(i, shadowMatrix), shaders, intensity);
                        }
                        else
                        {
                            DrawPartShadowed(3, whlId, this->GetWheelShadowMatrix(i, shadowMatrix), shaders, intensity);
                        }
                    }
                }
            }
        }

        //revert model faces back
        gfxRenderState::SetCullMode(prevCullMode);
        gfxRenderState::SetLighting(prevLighting);
    }

    AGE_API void vehTrailerInstance::DrawShadowMap()
    {
        hook::Thunk<0x4D8320>::Call<void>(this);
    }

    AGE_API void vehTrailerInstance::DrawGlow()
    {
        //don't draw trailer lights if it's broken
        if (this->GetTrailer()->GetJoint()->IsBroken())
            return;

        //get vars
        auto carsim = this->GetTrailer()->GetCarSim();
        float brakeInput = carsim->GetBrake();
        int gear = carsim->GetTransmission()->GetGear();

        //setup renderer
        gfxRenderState::SetWorldMatrix(this->GetTrailer()->GetInertialCS()->GetMatrix());

        //get shaders
        auto shaders = this->GetShader(this->Variant);

        //get lights
        modStatic* tlight = this->GetGeomEntry(this->GetGeomId("tlight"));
        modStatic* rlight = this->GetGeomEntry(this->GetGeomId("rlight"));
        modStatic* blight = this->GetGeomEntry(this->GetGeomId("blight"));
        modStatic* hlight = this->GetGeomEntry(this->GetGeomId("hlight"));
        modStatic* slight0 = this->GetGeomEntry(this->GetGeomId("slight0"));
        modStatic* slight1 = this->GetGeomEntry(this->GetGeomId("slight1"));
        modStatic* siren0 = this->GetGeomEntry(this->GetGeomId("siren0"));
        modStatic* siren1 = this->GetGeomEntry(this->GetGeomId("siren1"));
        modStatic* tslight0 = this->GetGeomEntry(this->GetGeomId("tslight0"));
        modStatic* tslight1 = this->GetGeomEntry(this->GetGeomId("tslight1"));

        if (vehCarModel::MM1StyleTransmission)
        {
            auto throttle = carsim->GetEngine()->GetThrottleInput();
            auto speedMPH = carsim->GetSpeedMPH();
            auto transmission = carsim->GetTransmission();

            //draw rlight
            if (rlight != nullptr && gear == 0)
            {
                if (transmission->IsAuto()) {
                    if (throttle > 0.f || speedMPH >= 1.f)
                        rlight->Draw(shaders);
                }
                else
                {
                    rlight->Draw(shaders);
                }
            }
        }
        else
        {
            //draw rlight
            if (rlight != nullptr && gear == 0)
                rlight->Draw(shaders);
        }

        //draw blight
        if (blight != nullptr && brakeInput > 0.1)
            blight->Draw(shaders);

        //draw tlight
        if (tlight != nullptr)
        {
            //draw night copy
            if (vehCarModel::ShowHeadlights)
                tlight->Draw(shaders);

            //draw brake input copy
            if (brakeInput > 0.1)
                tlight->Draw(shaders);
        }

        //draw hlight
        if (hlight != nullptr)
        {
            if (vehCarModel::ShowHeadlights)
                hlight->Draw(shaders);
        }

        //check signal clock
        bool drawSignal = fmod(datTimeManager::ElapsedTime + (float)this->GetRandId(), 1.f) > 0.5f;
        //draw stuff!
        if (drawSignal)
        {
            if (vehCarModel::LeftSignalLightState || vehCarModel::HazardLightsState)
            {
                if (slight0 != nullptr)
                    slight0->Draw(shaders);
                if (tslight0 != nullptr)
                    tslight0->Draw(shaders);
            }
            if (vehCarModel::RightSignalLightState || vehCarModel::HazardLightsState)
            {
                if (slight1 != nullptr)
                    slight1->Draw(shaders);
                if (tslight1 != nullptr)
                    tslight1->Draw(shaders);
            }
        }

        if (!vehCarModel::LeftSignalLightState && !vehCarModel::HazardLightsState)
        {
            if (tslight0 != nullptr) {
                //draw night copy
                if (vehCarModel::ShowHeadlights)
                    tslight0->Draw(shaders);

                //draw brake input copy
                if (brakeInput > 0.1) {
                    tslight0->Draw(shaders);
                }
            }
        }

        if (!vehCarModel::RightSignalLightState && !vehCarModel::HazardLightsState)
        {
            if (tslight1 != nullptr) {
                //draw night copy
                if (vehCarModel::ShowHeadlights)
                    tslight1->Draw(shaders);

                //draw brake input copy
                if (brakeInput > 0.1) {
                    tslight1->Draw(shaders);
                }
            }
        }

        //get vehSiren since vehTrailer doesn't have one by default
        auto mgr = *mmGameManager::Instance;
        auto game = mgr->getGame();
        auto player = game->GetPlayer();
        auto car = player->GetCar();
        auto siren = car->GetSiren();

        //draw siren
        if (siren != nullptr && siren->IsActive())
        {
            int sirenStage = fmod(datTimeManager::ElapsedTime + (float)this->GetRandId(), 2 * vehCarModel::SirenCycle) >= vehCarModel::SirenCycle ? 1 : 0;
            if (sirenStage == 0 && siren0 != nullptr)
                siren0->Draw(shaders);

            else if (sirenStage == 1 && siren1 != nullptr)
                siren1->Draw(shaders);
        }
    }

    AGE_API void vehTrailerInstance::DrawReflected(float intensity)
    {
        auto trailer = this->GetTrailer();
        auto trailerMatrix = trailer->GetInertialCS()->GetMatrix();

        //get shaders
        auto shaders = this->GetShader(this->Variant);

        //setup renderer
        gfxRenderState::SetWorldMatrix(trailerMatrix);

        //draw the trailer
        auto trailerGeom = this->GetGeom(3, 0);
        if (trailerGeom != nullptr)
            trailerGeom->DrawReflected(shaders, this->GetRoomId(), trailerMatrix, intensity);

        //get trailer wheels
        vehWheel* wheels[6] = { trailer->GetWheel(0), trailer->GetWheel(1), trailer->GetWheel(2), trailer->GetWheel(3), trailer->GetWheel(2), trailer->GetWheel(3) };

        //draw (s)whl0-5
        for (int i = 0; i < 6; i++)
        {
            string_buf<16> swhl("tswhl%d", i);
            string_buf<16> whl("twhl%d", i);
            int swhlId = this->GetGeomId(swhl);
            int whlId = this->GetGeomId(whl);
            auto wheel = wheels[i];
            auto swhlGeom = this->GetGeom(3, swhlId);
            if (fabs(wheel->GetRotationRate()) > BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
            {
                DrawPartReflected(3, swhlId, this->GetWheelMatrix(i), shaders, intensity, vehCarModel::WheelReflections);
            }
            else
            {
                DrawPartReflected(3, whlId, this->GetWheelMatrix(i), shaders, intensity, vehCarModel::WheelReflections);
            }
        }
    }

    AGE_API void vehTrailerInstance::DrawReflectedParts(int lod)
    {
        auto trailer = this->GetTrailer();
        auto trailerMatrix = trailer->GetInertialCS()->GetMatrix();

        //get shaders
        auto shaders = this->GetShader(this->Variant);

        //setup renderer
        gfxRenderState::SetWorldMatrix(trailerMatrix);

        //draw the trailer
        auto trailerGeom = this->GetGeom(lod, 0);
        if (trailerGeom != nullptr)
            trailerGeom->DrawGlass(shaders);
    }

    AGE_API unsigned int vehTrailerInstance::SizeOf(void)
    {
        return hook::Thunk<0x4D7850>::Call<unsigned int>(this);
    }

    AGE_API bool vehTrailerInstance::GetTrailerHitch(Vector3* out) const { return hook::Thunk<0x4D8420>::Call<bool>(this, out); }

    void vehTrailerInstance::BindLua(LuaState L) {
        LuaBinding(L).beginExtendClass<vehTrailerInstance, lvlInstance>("vehTrailerInstance")
            //properties
            .addPropertyReadOnly("Trailer", &GetTrailer)
            .addPropertyReadOnly("TrailerHitchOffset", &getTrailerHitchOffsetLua)
        .endClass();
    }
}