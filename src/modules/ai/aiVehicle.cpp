#include "aiVehicle.h"
#include "aiMap.h"
#include <modules\gfx\rstate.h>
#include <modules\city\citylevel.h>
#include <modules\mmcityinfo\state.h>

namespace MM2
{
    /*
        aiVehicleInstance
    */

    const float BlurSpeed = 20.0f;

    // Properties
    aiVehicleSpline* aiVehicleInstance::GetSpline()
    {
        return this->Spline;
    }

    int aiVehicleInstance::GetVariant() const
    {
        return this->Variant;
    }

    void aiVehicleInstance::DrawPartReflected(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity, bool reflected)
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

    void aiVehicleInstance::DrawPartShadowed(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity)
    {
        auto model = this->GetGeom(lod, geomId);
        if (model != nullptr)
        {
            gfxRenderState::SetWorldMatrix(matrix);
            model->DrawShadowed(shaders, intensity);
        }
    }

    void aiVehicleInstance::DrawPart(int lod, int geomId, const Matrix34& matrix, modShader* shaders, bool alphaState)
    {
        auto model = this->GetGeom(lod, geomId);
        if (model != nullptr)
            DrawPart(model, matrix, shaders, alphaState);
    }

    void aiVehicleInstance::DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders, bool alphaState)
    {
        gfxRenderState::SetWorldMatrix(matrix);
        alphaState ? model->DrawAlpha(shaders) : model->DrawNoAlpha(shaders);
    }

    void aiVehicleInstance::DrawHeadlights()
    {
        modStatic* headlight0 = this->GetGeomBase(HEADLIGHT0_GEOM_ID)->GetHighestLOD();
        if (headlight0 == nullptr)
            return;

        ltLight::DrawGlowBegin();

        auto light = (*aiVehicleManager::Instance)->GetSharedLight();
        auto lightPos = this->HeadlightPosition;

        Matrix34 vehicleMatrix = this->Spline->GetMatrix();
        light->Direction = Vector3(-vehicleMatrix.m20, -vehicleMatrix.m21, -vehicleMatrix.m22);

        //draw HEADLIGHT0
        light->Position = vehicleMatrix.Transform(lightPos);
        light->DrawGlow(static_cast<Vector3>(gfxRenderState::GetCameraMatrix().GetRow(3)));

        //draw HEADLIGHT1
        modStatic* headlight1 = this->GetGeomBase(HEADLIGHT1_GEOM_ID)->GetHighestLOD();
        if (headlight1 != nullptr)
        {
            lightPos.X *= -1.0f;
            light->Position = vehicleMatrix.Transform(lightPos);
            light->DrawGlow(static_cast<Vector3>(gfxRenderState::GetCameraMatrix().GetRow(3)));
        }

        ltLight::DrawGlowEnd();
    }

    Matrix34 aiVehicleInstance::GetWheelMatrix(int num)
    {
        Matrix34 whlMatrix = Matrix34();

        auto active = (aiVehicleActive*)this->GetEntity();
        if (active != nullptr)
        {
            if (num < 4)
            {
                return active->GetWheel(num)->GetMatrix();
            }
            else
            {
                auto wheel = active->GetWheel(num - 2);
                whlMatrix.Set(wheel->GetMatrix());
                whlMatrix.SetRow(3, whlMatrix.GetRow(3) + this->Spline->GetMatrix().Transform3x3(this->GetData()->GetWheelPosition(num) - wheel->GetPosition()));
            }
        }
        else
        {
            whlMatrix.Identity();
            whlMatrix.SetRow(3, this->GetData()->GetWheelPosition(num));
            whlMatrix.RotateX(-this->Spline->GetTireRotation());
            whlMatrix.Dot(this->Spline->GetMatrix());
        }
        
        return whlMatrix;
    }

    Matrix34 aiVehicleInstance::GetWheelShadowMatrix(int num, const Matrix34& shadowMatrix)
    {
        Matrix34 whlShadowMatrix = Matrix34();
        whlShadowMatrix.Identity();

        auto active = (aiVehicleActive*)this->GetEntity();
        if (active != nullptr)
        {
            if (num < 4)
            {
                whlShadowMatrix.SetRow(3, active->GetWheel(num)->GetPosition());
            }
            else
            {
                auto wheel = active->GetWheel(num - 2);
                whlShadowMatrix.SetRow(3, wheel->GetPosition());
                whlShadowMatrix.SetRow(3, whlShadowMatrix.GetRow(3) + (this->GetData()->GetWheelPosition(num) - wheel->GetPosition()));
            }
        }
        else
        {
            whlShadowMatrix.SetRow(3, this->GetData()->GetWheelPosition(num));
            whlShadowMatrix.RotateX(-this->Spline->GetTireRotation());
        }

        whlShadowMatrix.Dot(shadowMatrix);
        return whlShadowMatrix;
    }

    //overrides
    AGE_API Vector3 const& aiVehicleInstance::GetPosition()              { return hook::Thunk<0x553030>::Call<Vector3 const&>(this); };
    AGE_API Matrix34 const& aiVehicleInstance::GetMatrix(Matrix34* a1)   { return hook::Thunk<0x553020>::Call<Matrix34 const&>(this, a1); };
    AGE_API void aiVehicleInstance::SetMatrix(Matrix34 const & a1)       { hook::Thunk<0x553010>::Call<void>(this, a1); }
    
    AGE_API void aiVehicleInstance::SetVariant(int variant)
    {
        this->Variant = static_cast<short>(variant);
        this->GetGenBreakableMgr()->SetVariant(variant);
    }

    AGE_API dgPhysEntity* aiVehicleInstance::GetEntity()                 { return hook::Thunk<0x552F50>::Call<dgPhysEntity*>(this); };
    AGE_API dgPhysEntity* aiVehicleInstance::AttachEntity()              { return hook::Thunk<0x552FBD>::Call<dgPhysEntity*>(this); };
    AGE_API void aiVehicleInstance::Detach()                             { hook::Thunk<0x552F80>::Call<void>(this); }
        
    AGE_API void aiVehicleInstance::Draw(int lod)
    {
        if (lod < 0 || lod > 3)
            return;
        if (this->GetGeomIndex() == 0)
            return;

        //disable alpha drawing
        bool alphaDrawing = false;

        //get shaders
        auto shaders = this->GetShader(this->Variant);

        //setup renderer
        Matrix34 vehicleMatrix = this->Spline->GetMatrix();
        gfxRenderState::SetWorldMatrix(vehicleMatrix);

        //draw the body
        modStatic* bodyGeom = this->GetGeom(lod, 0);
        if (bodyGeom != nullptr)
            bodyGeom->DrawNoAlpha(shaders);

        //draw breakables
        this->BreakableMgr->Draw(vehicleMatrix, shaders, 3, alphaDrawing);

        //draw plights
        if (aiMap::GetInstance()->showHeadlights)
            //plighton
            DrawPart(3, PLIGHTON_GEOM_ID, vehicleMatrix, shaders, alphaDrawing);
        else
            //plightoff
            DrawPart(3, PLIGHTOFF_GEOM_ID, vehicleMatrix, shaders, alphaDrawing);

        //get wheel ids
        int swhlIds[6] = { SWHL0_GEOM_ID, SWHL1_GEOM_ID, SWHL2_GEOM_ID, SWHL3_GEOM_ID, SWHL4_GEOM_ID, SWHL5_GEOM_ID };
        int whlIds[6] = { WHL0_GEOM_ID, WHL1_GEOM_ID, WHL2_GEOM_ID, WHL3_GEOM_ID, WHL4_GEOM_ID, WHL5_GEOM_ID };

        //draw wheels (only in H LOD)
        if (lod == 3)
        {
            //(S)WHL0-5
            for (int i = 0; i < 6; i++)
            {
                int swhlId = swhlIds[i];
                int whlId = whlIds[i];
                auto swhlGeom = this->GetGeom(3, swhlId);
                if (this->Spline->GetSpeed() >= BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
                {
                    DrawPart(3, swhlId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                }
                else
                {
                    DrawPart(3, whlId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                }
            }
        }
    }

    AGE_API void aiVehicleInstance::DrawShadow()
    {
        if (vehCarModel::Enable3DShadows <= 2
            || MMSTATE->TimeOfDay == 3
            || lvlLevel::GetSingleton()->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
        {
            // draw drop shadow
            hook::Thunk<0x552CC0>::Call<void>(this);
            return;
        }

        auto timeWeather = cityLevel::GetCurrentLighting();

        bool prevLighting = gfxRenderState::SetLighting(true);

        //invert model faces
        auto prevCullMode = gfxRenderState::GetCullMode();
        gfxRenderState::SetCullMode(D3DCULL_CCW);

        //get shaders
        auto shaders = this->GetShader(this->GetVariant());

        //get model
        modStatic* model = this->GetGeomBase(0)->GetHighestLOD();

        if (model != nullptr)
        {
            Matrix34 shadowMatrix = Matrix34();
            Matrix34 vehicleMatrix = this->Spline->GetMatrix();

            if (lvlInstance::ComputeShadowProjectionMatrix(shadowMatrix, this->GetRoomId(), timeWeather->KeyPitch, timeWeather->KeyHeading, vehicleMatrix, this))
            {
                gfxRenderState::SetWorldMatrix(shadowMatrix);
                float intensity = ComputeShadowIntensity(timeWeather->KeyColor);
                model->DrawShadowed(shaders, intensity);

                //draw breakables
                this->BreakableMgr->DrawShadowed(shadowMatrix, shaders, intensity);

                //draw plights
                if (aiMap::GetInstance()->showHeadlights)
                    //plighton
                    DrawPartShadowed(3, PLIGHTON_GEOM_ID, shadowMatrix, shaders, intensity);
                else
                    //plightoff
                    DrawPartShadowed(3, PLIGHTOFF_GEOM_ID, shadowMatrix, shaders, intensity);

                //get wheel ids
                int whlIds[6] = { WHL0_GEOM_ID, WHL1_GEOM_ID, WHL2_GEOM_ID, WHL3_GEOM_ID, WHL4_GEOM_ID, WHL5_GEOM_ID };

                //draw wheels
                for (int i = 0; i < 6; i++)
                {
                    int whlGeomId = whlIds[i];
                    auto whlGeom = this->GetGeom(3, whlGeomId);
                    if (whlGeom != nullptr)
                    {
                        DrawPartShadowed(3, whlGeomId, this->GetWheelShadowMatrix(i, shadowMatrix), shaders, intensity);
                    }
                }
            }
        }

        //revert model faces back
        gfxRenderState::SetCullMode(prevCullMode);
        gfxRenderState::SetLighting(prevLighting);
    }

    AGE_API void aiVehicleInstance::DrawShadowMap()
    {
        hook::Thunk<0x552F30>::Call<void>(this);
    }

    AGE_API void aiVehicleInstance::DrawGlow()
    {
        if (this->GetGeomIndex() == 0)
            return;

        //setup renderer
        Matrix34 vehicleMatrix = this->Spline->GetMatrix();
        gfxRenderState::SetWorldMatrix(vehicleMatrix);

        //get shaders
        auto shaders = this->GetShader(this->Variant);

        //get objects
        modStatic* hlight = this->GetGeomBase(HLIGHT_GEOM_ID)->GetHighestLOD();
        modStatic* tlight = this->GetGeomBase(TLIGHT_GEOM_ID)->GetHighestLOD();
        modStatic* slight0 = this->GetGeomBase(SLIGHT0_GEOM_ID)->GetHighestLOD();
        modStatic* slight1 = this->GetGeomBase(SLIGHT1_GEOM_ID)->GetHighestLOD();
        modStatic* blight = this->GetGeomBase(BLIGHT_GEOM_ID)->GetHighestLOD();
        modStatic* tslight0 = this->GetGeomBase(TSLIGHT0_GEOM_ID)->GetHighestLOD();
        modStatic* tslight1 = this->GetGeomBase(TSLIGHT1_GEOM_ID)->GetHighestLOD();

        //get lights stuff
        float accel = this->Spline->GetRailSet()->GetAccelFactor();
        float speed = this->Spline->GetSpeed();

        //draw blight
        if (blight != nullptr)
        {
            if (accel < 0.f || speed == 0.f)
                blight->Draw(shaders);
        }

        //draw tlight
        if (tlight != nullptr)
        {
            //draw brake copy
            if (accel < 0.f || speed == 0.f)
                tlight->Draw(shaders);
            //draw headlight copy
            if (aiMap::GetInstance()->showHeadlights)
                tlight->Draw(shaders);
        }

        //draw signals
        if (this->SignalState & 1)
        {
            if ((aiVehicleManager::SignalClock + (int)this->RandomId) & 8)
            {
                if (slight0 != nullptr)
                    slight0->Draw(shaders);
                if (tslight0 != nullptr)
                    tslight0->Draw(shaders);
            }
        }
        else {
            if (tslight0 != nullptr)
            {
                //draw brake copy
                if (accel < 0.f || speed == 0.f)
                    tslight0->Draw(shaders);
                //draw headlight copy
                if (aiMap::GetInstance()->showHeadlights)
                    tslight0->Draw(shaders);
            }
        }

        if (this->SignalState & 2)
        {
            if ((aiVehicleManager::SignalClock + (int)this->RandomId) & 8)
            {
                if (slight1 != nullptr)
                    slight1->Draw(shaders);
                if (tslight1 != nullptr)
                    tslight1->Draw(shaders);
            }
        }
        else
        {
            if (tslight1 != nullptr)
            {
                //draw brake copy
                if (accel < 0.f || speed == 0.f)
                    tslight1->Draw(shaders);
                //draw headlight copy
                if (aiMap::GetInstance()->showHeadlights)
                    tslight1->Draw(shaders);
            }
        }

        //draw headlights
        if (aiVehicleInstance::AmbientHeadlightStyle < 3)
        {
            if (aiVehicleInstance::AmbientHeadlightStyle == 0 || aiVehicleInstance::AmbientHeadlightStyle == 2)
            {
                //MM2 headlights
                if (aiMap::GetInstance()->showHeadlights)
                    this->DrawHeadlights();
            }
            if (aiVehicleInstance::AmbientHeadlightStyle == 1 || aiVehicleInstance::AmbientHeadlightStyle == 2)
            {
                //MM1 headlights
                gfxRenderState::SetWorldMatrix(vehicleMatrix);

                if (hlight != nullptr && aiMap::GetInstance()->showHeadlights)
                    hlight->Draw(shaders);
            }
        }
    }

    AGE_API void aiVehicleInstance::DrawReflected(float intensity)
    {
        //get shaders
        auto shaders = this->GetShader(this->Variant);

        //setup renderer
        Matrix34 vehicleMatrix = this->Spline->GetMatrix();
        gfxRenderState::SetWorldMatrix(vehicleMatrix);

        //draw the body
        modStatic* bodyGeom = this->GetGeom(3, 0);
        if (bodyGeom != nullptr)
            bodyGeom->DrawReflected(shaders, this->GetRoomId(), vehicleMatrix, intensity);

        //draw breakables
        this->BreakableMgr->DrawReflected(vehicleMatrix, shaders, 3, this->GetRoomId());

        //draw plights
        if (aiMap::GetInstance()->showHeadlights)
            //plighton
            DrawPartReflected(3, PLIGHTON_GEOM_ID, vehicleMatrix, shaders, intensity, vehCarModel::PartReflections);
        else
            //plightoff
            DrawPartReflected(3, PLIGHTOFF_GEOM_ID, vehicleMatrix, shaders, intensity, vehCarModel::PartReflections);

        //get wheel ids
        int swhlIds[6] = { SWHL0_GEOM_ID, SWHL1_GEOM_ID, SWHL2_GEOM_ID, SWHL3_GEOM_ID, SWHL4_GEOM_ID, SWHL5_GEOM_ID };
        int whlIds[6] = { WHL0_GEOM_ID, WHL1_GEOM_ID, WHL2_GEOM_ID, WHL3_GEOM_ID, WHL4_GEOM_ID, WHL5_GEOM_ID };

        //draw (S)WHL0-5
        for (int i = 0; i < 6; i++)
        {
            int swhlId = swhlIds[i];
            int whlId = whlIds[i];
            auto swhlGeom = this->GetGeom(3, swhlId);
            if (this->Spline->GetSpeed() >= BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
            {
                DrawPartReflected(3, swhlId, this->GetWheelMatrix(i), shaders, intensity, vehCarModel::WheelReflections);
            }
            else
            {
                DrawPartReflected(3, whlId, this->GetWheelMatrix(i), shaders, intensity, vehCarModel::WheelReflections);
            }
        }
    }

    AGE_API void aiVehicleInstance::DrawReflectedParts(int lod)
    {
        if (lod < 0 || lod > 3)
            return;
        if (this->GetGeomIndex() == 0)
            return;

        //enable alpha drawing
        bool alphaDrawing = true;

        //get shaders
        auto shaders = this->GetShader(this->Variant);

        //setup renderer
        Matrix34 vehicleMatrix = this->Spline->GetMatrix();
        gfxRenderState::SetWorldMatrix(vehicleMatrix);

        //draw the body
        modStatic* bodyGeom = this->GetGeom(lod, 0);
        if (bodyGeom != nullptr)
            bodyGeom->DrawAlpha(shaders);

        //draw breakables
        this->BreakableMgr->Draw(vehicleMatrix, shaders, 3, alphaDrawing);

        //draw plights
        if (aiMap::GetInstance()->showHeadlights)
            //plighton
            DrawPart(3, PLIGHTON_GEOM_ID, vehicleMatrix, shaders, alphaDrawing);
        else
            //plightoff
            DrawPart(3, PLIGHTOFF_GEOM_ID, vehicleMatrix, shaders, alphaDrawing);

        //get wheel ids
        int swhlIds[6] = { SWHL0_GEOM_ID, SWHL1_GEOM_ID, SWHL2_GEOM_ID, SWHL3_GEOM_ID, SWHL4_GEOM_ID, SWHL5_GEOM_ID };
        int whlIds[6] = { WHL0_GEOM_ID, WHL1_GEOM_ID, WHL2_GEOM_ID, WHL3_GEOM_ID, WHL4_GEOM_ID, WHL5_GEOM_ID };

        //draw wheels (only in H LOD)
        if (lod == 3)
        {
            //(S)WHL0-5
            for (int i = 0; i < 6; i++)
            {
                int swhlId = swhlIds[i];
                int whlId = whlIds[i];
                auto swhlGeom = this->GetGeom(3, swhlId);
                if (this->Spline->GetSpeed() >= BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
                {
                    DrawPart(3, swhlId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                }
                else
                {
                    DrawPart(3, whlId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                }
            }
        }
    }

    AGE_API unsigned int aiVehicleInstance::SizeOf()                     { return hook::Thunk<0x553060>::Call<unsigned int>(this); };
    AGE_API phBound* aiVehicleInstance::GetBound(int a1)                 { return hook::Thunk<0x552F40>::Call<phBound*>(this, a1); };
        
    //members
    vehBreakableMgr* aiVehicleInstance::GetGenBreakableMgr()
    {
        return this->BreakableMgr;
    }

    aiVehicleData* aiVehicleInstance::GetData()                           { return hook::Thunk<0x553F80>::Call<aiVehicleData*>(this); }
    AGE_API void aiVehicleInstance::DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders, int unused)
                                                                          { hook::Thunk<0x552870>::Call<void>(this, model, &matrix, shaders, unused); }

    //lua
    void aiVehicleInstance::BindLua(LuaState L) {
        LuaBinding(L).beginExtendClass<aiVehicleInstance, lvlInstance>("aiVehicleInstance")
            //members
            .addPropertyReadOnly("Breakables", &GetGenBreakableMgr)
            .addProperty("Variant", &GetVariant, &SetVariant)
            .addFunction("GetData", &GetData)
            .endClass();
    }

    /*
        aiObstacle
    */
    BOOL aiObstacle::InAccident(void)                                    { return hook::Thunk<0x53F5D0>::Call<BOOL>(this); }
    float aiObstacle::BreakThreshold(void)                               { return hook::Thunk<0x53F5E0>::Call<float>(this); }
    BOOL aiObstacle::Drivable(void)                                      { return hook::Thunk<0x53F5F0>::Call<BOOL>(this); }

    //lua
    void aiObstacle::BindLua(LuaState L) {
        LuaBinding(L).beginClass<aiObstacle>("aiObstacle")
            .addPropertyReadOnly("BreakThreshold", &BreakThreshold)
            .addPropertyReadOnly("Speed", &Speed)
            .addPropertyReadOnly("InAccident", &luaInAccident)
            .addPropertyReadOnly("Drivable", &luaDrivable)
            .addFunction("IsBlockingTarget", &IsBlockingTarget)
            .addFunction("GetPosition", &luaPosition)
            .endClass();
    }

    /*
        aiVehicle
    */
    void aiVehicle::PreAvoid(const Vector3& a1, const Vector3& a2, float a3, Vector3& a4, Vector3& a5)
        FORWARD_THUNK;

    int aiVehicle::GetId() const
    {
        return this->Id;
    }

    float aiVehicle::IsBlockingTarget(const Vector3& a1, const Vector3& a2, float a3, float a4)
    {  return hook::Thunk<0x556850>::Call<float>(this, &a1, &a2, a3, a4); }

    void aiVehicle::Update(void) { return hook::Thunk<0x556230>::Call<void>(this); };
    void aiVehicle::Reset(void) { return hook::Thunk<0x556210>::Call<void>(this); };
    void aiVehicle::ReplayDebug(void) { return hook::Thunk<0x556D00>::Call<void>(this); }

    //lua
    void aiVehicle::BindLua(LuaState L) {
        LuaBinding(L).beginExtendClass<aiVehicle, aiObstacle>("aiVehicle")
            .addFunction("GetMatrix", &GetMatrix)
            .addFunction("DrawId", &DrawId)
            .addPropertyReadOnly("ID", &GetId)
            .addPropertyReadOnly("Type", &Type)
            .addPropertyReadOnly("FrontBumperDistance", &FrontBumperDistance)
            .addPropertyReadOnly("BackBumperDistance", &BackBumperDistance)
            .addPropertyReadOnly("BackBumperDistance", &BackBumperDistance)
            .addPropertyReadOnly("LSideDistance", &LSideDistance)
            .addPropertyReadOnly("RSideDistance", &RSideDistance)
            .addPropertyReadOnly("CurrentLane", &CurrentLane)
            .addPropertyReadOnly("CurrentRoadId", &CurrentRoadId)
            .endClass();
    }
}