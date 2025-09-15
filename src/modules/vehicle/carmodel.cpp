#pragma once
#include "carmodel.h"
#include <modules\city\citylevel.h>
#include <modules\mmcityinfo\state.h>

namespace MM2
{
    const float BlurSpeed = 26.0f;

    AGE_API vehCarModel::vehCarModel() {
        scoped_vtable x(this);
        hook::Thunk<0x4CCF20>::Call<void>(this);
    }

    AGE_API vehCarModel::~vehCarModel() {
        scoped_vtable x(this);
        hook::Thunk<0x4CCF80>::Call<void>(this);
    }

    vehBreakableMgr * vehCarModel::GetGenBreakableMgr()
    {
        return this->genBreakableMgr;
    }
        
    vehBreakableMgr * vehCarModel::GetMechBreakableMgr()
    {
        return this->wheelBreakableMgr;
    }

    vehCar * vehCarModel::GetCar()
    {
        return this->car;
    }

    int vehCarModel::GetVariant()
    {
        return this->variant;
    }

    AGE_API void vehCarModel::SetVariant(int variant)
    {
        this->variant = variant;
        this->PreLoadShader(variant);
        this->GetGenBreakableMgr()->SetVariant(variant);
        this->GetMechBreakableMgr()->SetVariant(variant);
        this->InitSirenLights(this->GetCar()->GetCarDamage()->GetName());
        this->InitHeadlights(this->GetCar()->GetCarDamage()->GetName());
        //this->Optimize(variant); (still crashy)

        // setup damage
        if (texelDamage) 
        {
            auto bodyEntry = this->GetGeomBase();
            if (bodyEntry->GetHighLOD() != nullptr)
            {
                delete texelDamage;
                texelDamage = new fxTexelDamage();
                texelDamage->Init(bodyEntry->GetHighLOD(), bodyEntry->pShaders[variant], bodyEntry->numShadersPerVariant);
            }
        }
        if (mm1Damage)
        {
            auto bodyEntry = this->GetGeomBase();
            if (bodyEntry->GetHighLOD() != nullptr)
            {
                delete mm1Damage;
                mm1Damage = new mmDamage();
                mm1Damage->Init(bodyEntry->GetHighLOD(), bodyEntry->pShaders[variant], bodyEntry->numShadersPerVariant);
            }
        }
        if (damage3D)
        {
            auto bodyEntry = this->GetGeomBase();
            auto bodyDamageEntry = this->GetGeomBase(BODYDAMAGE_GEOM_ID);
            if (bodyEntry->GetHighLOD() != nullptr)
            {
                delete damage3D;
                damage3D = new fxDamage3D();
                damage3D->Init(bodyEntry->GetHighLOD(), bodyDamageEntry->GetHighLOD() != nullptr ? bodyDamageEntry->GetHighLOD() : bodyEntry->GetHighLOD(), vehCarModel::mm1StyleDamage);
                damage3D->SetShaders(bodyEntry->pShaders[variant], bodyEntry->numShadersPerVariant);
            }
        }
    }

    ltLight* vehCarModel::GetHeadlight(int index)
    {
        //cap index
        if (index < 0)
            index = 0;
        if (index >= 2)
            index = 1;
            
        auto headlightLightsArray = *getPtr<ltLight*>(this, 0xB0);
        if (headlightLightsArray == nullptr)
            return NULL;
        return &headlightLightsArray[index];
    }

    int vehCarModel::GetWheelBrokenStatus()
    {
        return this->wheelBrokenStatus;
    }

    Vector3 vehCarModel::GetTrailerHitchOffset()
    {
        return this->trailerHitchPosition;
    }

    fxTexelDamage* vehCarModel::GetTexelDamage()
    {
        return texelDamage;
    }

    fxDamage3D* vehCarModel::GetDamage3D()
    {
        return damage3D;
    }

    mmDamage* vehCarModel::GetMM1Damage()
    {
        return mm1Damage;
    }

    int vehCarModel::GetRandId()
    {
        return irand2((int)this);
    }

    Matrix34 vehCarModel::GetWheelMatrix(int num)
    {
        Matrix34 whlMatrix = Matrix34();

        if (num < 4)
        {
            return this->carSim->GetWheel(num)->GetMatrix();
        }
        else
        {
            Matrix34 extraWhlMatrix = Matrix34();
            Vector3 extraWhlPosDiff[2] = { this->carSim->BackBackLeftWheelPosDiff, this->carSim->BackBackRightWheelPosDiff };
            extraWhlMatrix.Set(this->carSim->GetWheel(num - 2)->GetMatrix());
            extraWhlMatrix.SetRow(3, extraWhlMatrix.GetRow(3) + this->carSim->GetWorldMatrix()->Transform3x3(extraWhlPosDiff[num - 4]));
            whlMatrix.Set(extraWhlMatrix);
        }

        return whlMatrix;
    }

    Matrix34 vehCarModel::GetFenderMatrix(int num)
    {
        Matrix34 fndrMatrix = Matrix34();
        fndrMatrix.Identity();

        Matrix34 carMatrix = *this->carSim->GetWorldMatrix();
        fndrMatrix.SetRow(1, carMatrix.GetRow(1));

        Vector3 fndrOffsets[6] = { fndr0offset, fndr1offset, fndr2offset, fndr3offset, fndr4offset, fndr5offset };

        if (num < 4)
        {
            Matrix34 whlMatrix = this->carSim->GetWheel(num)->GetMatrix();
            fndrMatrix.SetRow(0, whlMatrix.GetRow(0));

            fndrMatrix.m20 = fndrMatrix.m12 * fndrMatrix.m01 - fndrMatrix.m11 * fndrMatrix.m02;
            fndrMatrix.m21 = fndrMatrix.m02 * fndrMatrix.m10 - fndrMatrix.m12 * fndrMatrix.m00;
            fndrMatrix.m22 = fndrMatrix.m11 * fndrMatrix.m00 - fndrMatrix.m01 * fndrMatrix.m10;

            fndrMatrix.SetRow(3, fndrMatrix.Transform3x3(fndrOffsets[num]) + whlMatrix.GetRow(3));
        }
        else
        {
            Matrix34 whlMatrix = this->GetWheelMatrix(num);
            fndrMatrix.SetRow(0, whlMatrix.GetRow(0));

            fndrMatrix.m20 = fndrMatrix.m12 * fndrMatrix.m01 - fndrMatrix.m11 * fndrMatrix.m02;
            fndrMatrix.m21 = fndrMatrix.m02 * fndrMatrix.m10 - fndrMatrix.m12 * fndrMatrix.m00;
            fndrMatrix.m22 = fndrMatrix.m11 * fndrMatrix.m00 - fndrMatrix.m01 * fndrMatrix.m10;

            fndrMatrix.SetRow(3, fndrMatrix.Transform3x3(fndrOffsets[num]) + whlMatrix.GetRow(3));
        }

        return fndrMatrix;
    }

    Matrix34 vehCarModel::GetWheelShadowMatrix(int num, const Matrix34& shadowMatrix)
    {
        Vector3 extraWhlPosDiff[2] = { this->carSim->BackBackLeftWheelPosDiff, this->carSim->BackBackRightWheelPosDiff };
        Matrix34 whlShadowMatrix = Matrix34();
        whlShadowMatrix.Identity();

        if (num < 4)
        {
            auto wheel = this->carSim->GetWheel(num);
            whlShadowMatrix.SetRow(3, wheel->GetCenter());
            whlShadowMatrix.Rotate(whlShadowMatrix.GetRow(0), wheel->GetAccumulatedRotation());
            whlShadowMatrix.RotateY(wheel->GetSteerAmount());
            whlShadowMatrix.Rotate(whlShadowMatrix.GetRow(2), wheel->GetWobbleAmount());
            whlShadowMatrix.m31 += wheel->GetBumpDisplacement();
        }
        else
        {
            auto wheel = this->carSim->GetWheel(num - 2);
            whlShadowMatrix.SetRow(3, extraWhlPosDiff[num - 4]);
            whlShadowMatrix.Rotate(whlShadowMatrix.GetRow(0), wheel->GetAccumulatedRotation());
            whlShadowMatrix.RotateY(wheel->GetSteerAmount());
            whlShadowMatrix.Rotate(whlShadowMatrix.GetRow(2), wheel->GetWobbleAmount());
            whlShadowMatrix.m31 += wheel->GetBumpDisplacement();
        }

        whlShadowMatrix.Dot(shadowMatrix);
        return whlShadowMatrix;
    }

    Matrix34 vehCarModel::GetFenderShadowMatrix(int num, const Matrix34 &shadowMatrix)
    {
        Matrix34 fndrShadowMatrix = Matrix34();
        fndrShadowMatrix.Identity();

        Vector3 fndrOffsets[6] = { fndr0offset, fndr1offset, fndr2offset, fndr3offset, fndr4offset, fndr5offset };
        Vector3 extraWhlPosDiff[2] = { this->carSim->BackBackLeftWheelPosDiff, this->carSim->BackBackRightWheelPosDiff };

        if (num < 4)
        {
            auto wheel = this->carSim->GetWheel(num);
            fndrShadowMatrix.SetRow(3, fndrOffsets[num] + wheel->GetCenter());
            fndrShadowMatrix.RotateY(wheel->GetSteerAmount());
            fndrShadowMatrix.Rotate(wheel->GetMatrix().GetRow(2), wheel->GetWobbleAmount());
            fndrShadowMatrix.m31 += wheel->GetBumpDisplacement();
        }
        else
        {
            auto wheel = this->carSim->GetWheel(num - 2);
            fndrShadowMatrix.SetRow(3, fndrOffsets[num] + extraWhlPosDiff[num - 4]);
            fndrShadowMatrix.RotateY(wheel->GetSteerAmount());
            fndrShadowMatrix.Rotate(wheel->GetMatrix().GetRow(2), wheel->GetWobbleAmount());
            fndrShadowMatrix.m31 += wheel->GetBumpDisplacement();
        }

        fndrShadowMatrix.Dot(shadowMatrix);
        return fndrShadowMatrix;
    }

    AGE_API void vehCarModel::GetSurfaceColor(modStatic* model, Vector3* outVector)
                                                        { hook::Thunk<0x4CDF00>::Call<void>(this, model, outVector); }
    AGE_API void vehCarModel::InitBreakable(vehBreakableMgr* manager, const char* basename, const char* breakableName, int geomId, int someId)
                                                        { hook::Thunk<0x4CDC50>::Call<void>(this, manager, basename, breakableName, geomId, someId); }
    AGE_API void vehCarModel::InitSirenLight(const char* basename, const char* mtxName, int geomId)
    {
        Vector3 outColor = Vector3::ORIGIN;
        auto sirenGeom = this->GetGeom(3, geomId);
        if (sirenGeom != nullptr)
        {
            auto siren = this->car->GetSiren();
            Matrix34 outMatrix;

            GetPivot(outMatrix, basename, mtxName);
            this->GetSurfaceColor(sirenGeom, &outColor);
            siren->AddLight(outMatrix.GetRow(3), outColor);
        }    
    }

    void vehCarModel::InitSirenLights(const char* basename)
    {
        auto siren = this->car->GetSiren();
        if (siren != nullptr)
        {
            siren->Init();
            siren->RemoveAllLights();
            for (int i = 0; i < 4; i++)
            {
                string_buf<16> buffer("srn%d", i);
                InitSirenLight(basename, buffer, SRN0_GEOM_ID + i);
            }
            for (int i = 4; i < 8; i++)
            {
                string_buf<16> buffer("srn%d", i);
                InitSirenLight(basename, buffer, SRN4_GEOM_ID + i - 4);
            }
        }
    }

    void vehCarModel::InitHeadlights(const char* basename)
    {
        Matrix34 outMatrix = Matrix34::I;

        // load headlights
        auto headlight0geom = this->GetGeom(3, HEADLIGHT0_GEOM_ID);
        auto headlight1geom = this->GetGeom(3, HEADLIGHT1_GEOM_ID);

        if (this->headlights)
        {
            delete[] this->headlights;
            this->headlights = nullptr;
        }
        if (headlight0geom != nullptr && headlight1geom != nullptr)
        {
            this->headlights = new ltLight[2];
            
            GetPivot(outMatrix, basename, "headlight0");
            headlights[0].Color = Vector3(1.f, 1.f, 1.f);
            headlights[0].Type = 1;
            headlights[0].SpotExponent = 3.f;
            this->GetSurfaceColor(headlight0geom, &headlights[0].Color);
            this->headlightPositions[0] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);

            GetPivot(outMatrix, basename, "headlight1");
            headlights[1].Color = Vector3(1.f, 1.f, 1.f);
            headlights[1].Type = 1;
            headlights[1].SpotExponent = 3.f;
            this->GetSurfaceColor(headlight1geom, &headlights[1].Color);
            this->headlightPositions[1] = outMatrix.GetRow(3);
        }

        // load extra headlights
        int EXTRA_HEADLIGHT_COUNT = 6; 
        for (int i = 0; i < EXTRA_HEADLIGHT_COUNT; i++)
        {
            if (extraHeadlights[i])
            {
                delete extraHeadlights[i];
                extraHeadlights[i] = nullptr;
            }
        }

        for (int i = 0; i < EXTRA_HEADLIGHT_COUNT; i++)
        {
            auto headlightGeom = this->GetGeom(3, HEADLIGHT2_GEOM_ID + i);
            if (headlightGeom == nullptr)
                continue;

            Matrix34 outMatrix;

            string_buf<16> buffer("headlight%d", i + 2);
            GetPivot(outMatrix, basename, buffer);
            extraHeadlights[i] = new ltLight();
            extraHeadlights[i]->Color = Vector3(1.f, 1.f, 1.f);
            extraHeadlights[i]->Type = 1;
            extraHeadlights[i]->SpotExponent = 3.f;
            this->GetSurfaceColor(headlightGeom, &extraHeadlights[i]->Color);
            this->extraHeadlightPositions[i] = outMatrix.GetRow(3);
        }
    }

    AGE_API void vehCarModel::BreakElectrics(Vector3* localImpactPos)     
    {
        hook::Thunk<0x4CEFE0>::Call<void>(this, localImpactPos); 
    }
    
    AGE_API void vehCarModel::ClearDamage()                          
    {
        genBreakableMgr->Reset();
        wheelBreakableMgr->Reset();
        if (texelDamage)
            texelDamage->Reset();
        if (damage3D)
            damage3D->SetNoDamage();
        if (mm1Damage)
            mm1Damage->Reset(false);
        
        this->hasEjectedOneshot = false;
        this->wheelBrokenStatus = 0xFFFFFFFF;
        this->enabledElectrics[0] = true;
        this->enabledElectrics[1] = true;
        this->enabledElectrics[2] = true;
        this->enabledElectrics[3] = true;
        
    }

    AGE_API void vehCarModel::EjectOneshot()
    {   
        if (!hasEjectedOneshot) {
            if (this->carSim->GetSpeedMPH() > 100.f) {
                this->wheelBreakableMgr->EjectAll(this->GetRoomId());
                this->wheelBrokenStatus = 0;
                this->hasEjectedOneshot = true;
                return;
            }
            if (this->carSim->GetSpeedMPH() <= 75.f) {
                if (this->carSim->GetSpeedMPH() <= 50.f)
                    return;

                int i = 3 * (irand() % 4);

                int wheelStatusFlag = 1 << i;
                int hubStatusFlag = 1 << (i + 1);
                int fenderStatusFlag = 1 << (i + 2);

                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(wheelStatusFlag), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(hubStatusFlag), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(fenderStatusFlag), this->GetRoomId());

                int ejectPackage = (wheelStatusFlag | hubStatusFlag | fenderStatusFlag);

                int ii = 3 * (irand() % 2);

                int extraWheelStatusFlag = 1 << (ii + 12);
                int extraHubStatusFlag = 1 << (ii + 13);
                int extraFenderStatusFlag = 1 << (ii + 14);

                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraWheelStatusFlag), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraHubStatusFlag), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraFenderStatusFlag), this->GetRoomId());

                int ejectPackage2 = (extraWheelStatusFlag | extraHubStatusFlag | extraFenderStatusFlag);

                this->wheelBrokenStatus &= ~(ejectPackage | ejectPackage2);
                this->hasEjectedOneshot = true;
                return;
            }
            else {
                int i = 3 * (irand() % 4);

                int wheelStatusFlag = 1 << i;
                int hubStatusFlag = 1 << (i + 1);
                int fenderStatusFlag = 1 << (i + 2);

                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(wheelStatusFlag), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(hubStatusFlag), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(fenderStatusFlag), this->GetRoomId());

                int ejectPackage = (wheelStatusFlag | hubStatusFlag | fenderStatusFlag);

                int ii = 3 * (irand() % 4);

                int wheelStatusFlag2 = 1 << ii;
                int hubStatusFlag2 = 1 << (ii + 1);
                int fenderStatusFlag2 = 1 << (ii + 2);

                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(wheelStatusFlag2), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(hubStatusFlag2), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(fenderStatusFlag2), this->GetRoomId());

                int ejectPackage2 = (wheelStatusFlag2 | hubStatusFlag2 | fenderStatusFlag2);

                int iii = 3 * (irand() % 2);

                int extraWheelStatusFlag = 1 << (iii + 12);
                int extraHubStatusFlag = 1 << (iii + 13);
                int extraFenderStatusFlag = 1 << (iii + 14);

                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraWheelStatusFlag), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraHubStatusFlag), this->GetRoomId());
                this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraFenderStatusFlag), this->GetRoomId());

                int ejectPackage3 = (extraWheelStatusFlag | extraHubStatusFlag | extraFenderStatusFlag);

                this->wheelBrokenStatus &= ~(ejectPackage | ejectPackage2 | ejectPackage3);
                this->hasEjectedOneshot = true;
                return;
            }
        }
    }

    AGE_API bool vehCarModel::GetVisible()                           { return hook::Thunk<0x4CF070>::Call<bool>(this); }
    AGE_API void vehCarModel::SetVisible(bool visible)               { hook::Thunk<0x4CF050>::Call<void>(this, visible); }
        
    AGE_API void vehCarModel::DrawHeadlights(bool rotate)
    {
        if (this->headlights == nullptr)
            return;

        if (rotate)
        {
            this->headlights[0].Direction.RotateY(datTimeManager::Seconds * vehCarModel::HeadlightFlashingSpeed);
            this->headlights[1].Direction.RotateY(datTimeManager::Seconds * -vehCarModel::HeadlightFlashingSpeed);
        }
        else
        {
            auto carMatrix = this->carSim->GetWorldMatrix();
            this->headlights[0].Direction = Vector3(-carMatrix->m20, -carMatrix->m21, -carMatrix->m22);
            this->headlights[1].Direction = Vector3(-carMatrix->m20, -carMatrix->m21, -carMatrix->m22);
        }

        bool bothLightsBroken = !(enabledElectrics[2] || enabledElectrics[3]);
        if (bothLightsBroken)
            return;

        ltLight::DrawGlowBegin();
        for (int i = 0; i < 2; i++)
        {
            bool isHeadlightBroken = !(enabledElectrics[i + 2]);
            if (isHeadlightBroken)
                continue;

            auto light = &this->headlights[i];
            auto lightPos = this->headlightPositions[i];
            auto carMatrix = this->carSim->GetWorldMatrix();

            light->Position = carMatrix->Transform(lightPos);
            light->DrawGlow(static_cast<Vector3>(gfxRenderState::GetCameraMatrix().GetRow(3)));
        }
        ltLight::DrawGlowEnd();
    }

    AGE_API void vehCarModel::DrawExtraHeadlights(bool rotate)
    {
        int geomSetId = this->GetGeomIndex();
        int geomSetIdOffset = geomSetId - 1;
        float rotationAmount = vehCarModel::HeadlightFlashingSpeed;

        ltLight::DrawGlowBegin();
        for (int i = 0; i < 6; i++)
        {
            auto headlightEntry = this->GetGeomBase(vehCarModel::HEADLIGHT2_GEOM_ID + i);
            if (headlightEntry == nullptr || headlightEntry->GetHighLOD() == nullptr)
                continue;

            if (rotate)
            {
                this->extraHeadlights[i]->Direction.RotateY(datTimeManager::Seconds * rotationAmount);
                rotationAmount *= -1.f;
            }
            else
            {
                auto carMatrix = this->carSim->GetWorldMatrix();
                this->extraHeadlights[i]->Direction = Vector3(-carMatrix->m20, -carMatrix->m21, -carMatrix->m22);
            }

            auto light = this->extraHeadlights[i];
            auto lightPos = this->extraHeadlightPositions[i];
            auto carMatrix = this->carSim->GetWorldMatrix();

            light->Position = carMatrix->Transform(lightPos);
            light->DrawGlow(static_cast<Vector3>(gfxRenderState::GetCameraMatrix().GetRow(3)));
        }
        ltLight::DrawGlowEnd();
    }
        
    AGE_API void vehCarModel::DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders)
    {
        gfxRenderState::SetWorldMatrix(matrix);
        model->Draw(shaders);
    }

    AGE_API void vehCarModel::DrawPart(int lod, int geomId, const Matrix34& matrix, modShader* shaders)
    {
        auto model = this->GetGeom(lod, geomId);
        if (model != nullptr)
            DrawPart(model, matrix, shaders);
    }

    void vehCarModel::DrawPartReflected(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity, bool reflected)
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

    void vehCarModel::DrawPartShadowed(int lod, int geomId, const Matrix34& matrix, modShader* shaders, float intensity)
    {
        auto model = this->GetGeom(lod, geomId);
        if (model != nullptr)
        {
            gfxRenderState::SetWorldMatrix(matrix);
            model->DrawShadowed(shaders, intensity);
        }
    }

    void vehCarModel::DrawPart(int lod, int geomId, const Matrix34& matrix, modShader* shaders, bool alphaState)
    {
        auto model = this->GetGeom(lod, geomId);
        if (model != nullptr)
            DrawPart(model, matrix, shaders, alphaState);
    }

    void vehCarModel::DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders, bool alphaState)
    {
        gfxRenderState::SetWorldMatrix(matrix);
        alphaState ? model->DrawAlpha(shaders) : model->DrawNoAlpha(shaders);
    }

    //new init
    void vehCarModel::Init(vehCar* car, const char* basename, int variant)
    {
        this->car = car;
        this->carSim = car->GetCarSim();
        this->variant = (variant > 255) ? 0 : variant;
        bool hasGeometry = false;

        if (lvlInstance::BeginGeomWithGroup(basename, "body", "player", 0xC)) 
        {
            gfxPacket::gfxForceLVERTEX = true;
            lvlInstance::AddGeom(basename, "shadow", 0);
            lvlInstance::AddGeom(basename, "hlight", 0);
            lvlInstance::AddGeom(basename, "tlight", 0);
            lvlInstance::AddGeom(basename, "rlight", 0);
            lvlInstance::AddGeom(basename, "slight0", 0);
            lvlInstance::AddGeom(basename, "slight1", 0);
            lvlInstance::AddGeom(basename, "blight", 0);
            gfxPacket::gfxForceLVERTEX = false;

            lvlInstance::AddGeom(basename, "bodydamage", 0);
            lvlInstance::AddGeom(basename, "siren0", 0);
            lvlInstance::AddGeom(basename, "siren1", 0);
            lvlInstance::AddGeom(basename, "decal", 0);
            lvlInstance::AddGeom(basename, "driver", 0);
            lvlInstance::AddGeom(basename, "shock0", 0);
            lvlInstance::AddGeom(basename, "shock1", 0);
            lvlInstance::AddGeom(basename, "shock2", 0);
            lvlInstance::AddGeom(basename, "shock3", 0);
            lvlInstance::AddGeom(basename, "arm0", 0);
            lvlInstance::AddGeom(basename, "arm1", 0);
            lvlInstance::AddGeom(basename, "arm2", 0);
            lvlInstance::AddGeom(basename, "arm3", 0);
            lvlInstance::AddGeom(basename, "shaft2", 0);
            lvlInstance::AddGeom(basename, "shaft3", 0);
            lvlInstance::AddGeom(basename, "axle0", 0);
            lvlInstance::AddGeom(basename, "axle1", 0);
            lvlInstance::AddGeom(basename, "engine", 2);

            lvlInstance::AddGeom(basename, "whl0", 6);
            lvlInstance::AddGeom(basename, "whl1", 4);
            lvlInstance::AddGeom(basename, "whl2", 4);
            lvlInstance::AddGeom(basename, "whl3", 4);

            lvlInstance::AddGeom(basename, "break0", 2);
            lvlInstance::AddGeom(basename, "break1", 2);
            lvlInstance::AddGeom(basename, "break2", 2);
            lvlInstance::AddGeom(basename, "break3", 2);
            lvlInstance::AddGeom(basename, "break01", 2);
            lvlInstance::AddGeom(basename, "break12", 2);
            lvlInstance::AddGeom(basename, "break23", 2);
            lvlInstance::AddGeom(basename, "break03", 2);

            lvlInstance::AddGeom(basename, "hub0", 0);
            lvlInstance::AddGeom(basename, "hub1", 0);
            lvlInstance::AddGeom(basename, "hub2", 0);
            lvlInstance::AddGeom(basename, "hub3", 0);

            lvlInstance::AddGeom(basename, "trailer_hitch", 0);

            lvlInstance::AddGeom(basename, "srn0", 0);
            lvlInstance::AddGeom(basename, "srn1", 0);
            lvlInstance::AddGeom(basename, "srn2", 0);
            lvlInstance::AddGeom(basename, "srn3", 0);

            gfxPacket::gfxForceLVERTEX = true;
            lvlInstance::AddGeom(basename, "headlight0", 0);
            lvlInstance::AddGeom(basename, "headlight1", 0);
            gfxPacket::gfxForceLVERTEX = false;

            lvlInstance::AddGeom(basename, "fndr0", 0);
            lvlInstance::AddGeom(basename, "fndr1", 0);
            lvlInstance::AddGeom(basename, "whl4", 0);
            lvlInstance::AddGeom(basename, "whl5", 0);

            //NEW MM2HOOK OBJECTS
            lvlInstance::AddGeom(basename, "plighton", 0);
            lvlInstance::AddGeom(basename, "plightoff", 0);

            lvlInstance::AddGeom(basename, "swhl0", 0);
            lvlInstance::AddGeom(basename, "swhl1", 0);
            lvlInstance::AddGeom(basename, "swhl2", 0);
            lvlInstance::AddGeom(basename, "swhl3", 0);
            lvlInstance::AddGeom(basename, "swhl4", 0);
            lvlInstance::AddGeom(basename, "swhl5", 0);

            lvlInstance::AddGeom(basename, "hub4", 0);
            lvlInstance::AddGeom(basename, "hub5", 0);

            lvlInstance::AddGeom(basename, "fndr2", 0);
            lvlInstance::AddGeom(basename, "fndr3", 0);
            lvlInstance::AddGeom(basename, "fndr4", 0);
            lvlInstance::AddGeom(basename, "fndr5", 0);

            lvlInstance::AddGeom(basename, "shub0", 0);
            lvlInstance::AddGeom(basename, "shub1", 0);
            lvlInstance::AddGeom(basename, "shub2", 0);
            lvlInstance::AddGeom(basename, "shub3", 0);
            lvlInstance::AddGeom(basename, "shub4", 0);
            lvlInstance::AddGeom(basename, "shub5", 0);

            gfxPacket::gfxForceLVERTEX = true;
            lvlInstance::AddGeom(basename, "headlight2", 0);
            lvlInstance::AddGeom(basename, "headlight3", 0);
            lvlInstance::AddGeom(basename, "headlight4", 0);
            lvlInstance::AddGeom(basename, "headlight5", 0);
            lvlInstance::AddGeom(basename, "headlight6", 0);
            lvlInstance::AddGeom(basename, "headlight7", 0);
            gfxPacket::gfxForceLVERTEX = false;

            lvlInstance::AddGeom(basename, "srn4", 0);
            lvlInstance::AddGeom(basename, "srn5", 0);
            lvlInstance::AddGeom(basename, "srn6", 0);
            lvlInstance::AddGeom(basename, "srn7", 0);
            lvlInstance::AddGeom(basename, "lightbar0", 0);
            lvlInstance::AddGeom(basename, "lightbar1", 0);

            gfxPacket::gfxForceLVERTEX = true;
            lvlInstance::AddGeom(basename, "tslight0", 0);
            lvlInstance::AddGeom(basename, "tslight1", 0);
            gfxPacket::gfxForceLVERTEX = false;

            //add variants
            //supports up to 16 paintjobs
            for (int i = 0; i < 16; i++)
            {
                string_buf<16> buffer("variant%d", i);
                lvlInstance::AddGeom(basename, buffer, (i == this->variant) ? 2 : 8);
            }

            lvlInstance::EndGeom();
            hasGeometry = true;
        }

        //clamp variant value
        this->variant = this->variant % this->GetVariantCount();

        //get our geometry id
        int geomSetId = this->GetGeomIndex();
        int geomSetIdOffset = geomSetId - 1;

        //pre-load our variant
        lvlInstance::PreLoadShader(this->variant);

        if (!vehCarModel::mm1StyleDamage)
        {
            //init fxTexelDamage
            if (this->texelDamage != nullptr)
            {
                // delete old texel
                delete this->texelDamage;
                this->texelDamage = nullptr;
            }

            if (this->GetGeomIndex() != 0)
            {
                auto bodyEntry = this->GetGeomBase();
                if (bodyEntry->GetHighLOD() != nullptr)
                {
                    this->texelDamage = new fxTexelDamage();
                    if (!texelDamage->Init(bodyEntry->GetHighLOD(), bodyEntry->pShaders[this->GetVariant()], bodyEntry->numShadersPerVariant))
                    {
                        delete texelDamage;
                        texelDamage = nullptr;
                        Warningf("Vehicle '%s': No damage textures found, disabling texel damage.", basename);
                    }
                }
            }
        }

        if (vehCarModel::mm1StyleDamage && !vehCarModel::Enable3DDamage)
        {
            //init mm1Damage
            if (this->mm1Damage != nullptr)
            {
                // delete old mm1Damage
                delete this->mm1Damage;
                this->mm1Damage = nullptr;
            }

            if (this->GetGeomIndex() != 0)
            {
                auto bodyEntry = this->GetGeomBase();
                if (bodyEntry->GetHighLOD() != nullptr)
                {
                    this->mm1Damage = new mmDamage();
                    mm1Damage->Init(bodyEntry->GetHighLOD(), bodyEntry->pShaders[this->GetVariant()], bodyEntry->numShadersPerVariant);
                }
            }
        }

        if (vehCarModel::Enable3DDamage)
        {
            //init damage3d
            if (this->damage3D != nullptr)
            {
                // delete old damage3D
                delete this->damage3D;
                this->damage3D = nullptr;
            }

            if (this->GetGeomIndex() != 0)
            {
                auto bodyEntry = this->GetGeomBase(0);
                auto bodyDamageEntry = this->GetGeomBase(BODYDAMAGE_GEOM_ID);
                if (bodyEntry->GetHighLOD() != nullptr)
                {
                    this->damage3D = new fxDamage3D();
                    damage3D->Init(bodyEntry->GetHighLOD(), bodyDamageEntry->GetHighLOD() != nullptr ? bodyDamageEntry->GetHighLOD() : bodyEntry->GetHighLOD(), vehCarModel::mm1StyleDamage);
                    damage3D->SetShaders(bodyEntry->pShaders[this->GetVariant()], bodyEntry->numShadersPerVariant);
                }
            }
        }

        //optimize this instance
        if (hasGeometry)
            lvlInstance::Optimize(this->variant);

        //init siren lights and headlights
        InitSirenLights(basename);
        InitHeadlights(basename);

        //load FNDR offsets
        if (this->GetGeom(3, FNDR0_GEOM_ID) != nullptr)
        {
            Matrix34 outMatrix;
            auto carsim = this->carSim;
                    
            //note about the 0.025y offset
            //the game has this weird offset by default
            //removing it appears to have no functional issues, but visually
            //mods will be broken if removed because they compensate
            GetPivot(outMatrix, basename, "fndr0");
            this->fndr0offset = outMatrix.GetRow(3) - carsim->GetWheel(0)->GetCenter();
            this->fndr0offset.Y += 0.025f;

            GetPivot(outMatrix, basename, "fndr1");
            this->fndr1offset = outMatrix.GetRow(3) - carsim->GetWheel(1)->GetCenter();
            this->fndr1offset.Y += 0.025f;
        }

        //extra FNDR2/3 offsets
        if (this->GetGeom(3, FNDR2_GEOM_ID) != nullptr)
        {
            Matrix34 outMatrix;
            auto carsim = this->carSim;

            GetPivot(outMatrix, basename, "fndr2");
            this->fndr2offset = outMatrix.GetRow(3) - carsim->GetWheel(2)->GetCenter();

            GetPivot(outMatrix, basename, "fndr3");
            this->fndr3offset = outMatrix.GetRow(3) - carsim->GetWheel(3)->GetCenter();
        }

        //extra FNDR4/5 offsets
        if (this->GetGeom(3, FNDR4_GEOM_ID) != nullptr)
        {
            Matrix34 outMatrix;
            auto carsim = this->carSim;

            GetPivot(outMatrix, basename, "fndr4");
            this->fndr4offset = outMatrix.GetRow(3) - carsim->GetWheel(2)->GetCenter();
            this->fndr4offset.Z += 1.10738f;

            GetPivot(outMatrix, basename, "fndr5");
            this->fndr5offset = outMatrix.GetRow(3) - carsim->GetWheel(3)->GetCenter();
            this->fndr5offset.Z += 1.10738f;
        }

        //create gen breakables
        this->genBreakableMgr = new vehBreakableMgr();
        this->genBreakableMgr->Init(this->carSim->GetWorldMatrix());
        this->genBreakableMgr->SetVariant(this->variant);

        InitBreakable(this->genBreakableMgr, basename, "break0", BREAK0_GEOM_ID, 0);
        InitBreakable(this->genBreakableMgr, basename, "break1", BREAK1_GEOM_ID, 0);
        InitBreakable(this->genBreakableMgr, basename, "break2", BREAK2_GEOM_ID, 0);
        InitBreakable(this->genBreakableMgr, basename, "break3", BREAK3_GEOM_ID, 0);
        InitBreakable(this->genBreakableMgr, basename, "break01", BREAK01_GEOM_ID, 0);
        InitBreakable(this->genBreakableMgr, basename, "break12", BREAK12_GEOM_ID, 0);
        InitBreakable(this->genBreakableMgr, basename, "break23", BREAK23_GEOM_ID, 0);
        InitBreakable(this->genBreakableMgr, basename, "break03", BREAK03_GEOM_ID, 0);
        InitBreakable(this->genBreakableMgr, basename, "lightbar0", LIGHTBAR0_GEOM_ID, 1);
        InitBreakable(this->genBreakableMgr, basename, "lightbar1", LIGHTBAR1_GEOM_ID, 2);
            
        int variantGeomId = this->variant + VARIANT_BASE_GEOM_ID;
        string_buf<16> buffer("variant%d", this->variant);
        InitBreakable(this->genBreakableMgr, basename, buffer, variantGeomId, 0);

        //create wheel breakables
        this->wheelBreakableMgr = new vehBreakableMgr();
        this->wheelBreakableMgr->Init(this->carSim->GetWorldMatrix());
        this->wheelBreakableMgr->SetVariant(this->variant);
            
        InitBreakable(this->wheelBreakableMgr, basename, "whl0", WHL0_GEOM_ID, 1 << 0);
        InitBreakable(this->wheelBreakableMgr, basename, "hub0", HUB0_GEOM_ID, 1 << 1);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr0", FNDR0_GEOM_ID, 1 << 2);
        InitBreakable(this->wheelBreakableMgr, basename, "whl1", WHL1_GEOM_ID, 1 << 3);
        InitBreakable(this->wheelBreakableMgr, basename, "hub1", HUB1_GEOM_ID, 1 << 4);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr1", FNDR1_GEOM_ID, 1 << 5);
        InitBreakable(this->wheelBreakableMgr, basename, "whl2", WHL2_GEOM_ID, 1 << 6);
        InitBreakable(this->wheelBreakableMgr, basename, "hub2", HUB2_GEOM_ID, 1 << 7);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr2", FNDR2_GEOM_ID, 1 << 8);
        InitBreakable(this->wheelBreakableMgr, basename, "whl3", WHL3_GEOM_ID, 1 << 9);
        InitBreakable(this->wheelBreakableMgr, basename, "hub3", HUB3_GEOM_ID, 1 << 10);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr3", FNDR3_GEOM_ID, 1 << 11);
        InitBreakable(this->wheelBreakableMgr, basename, "whl4", WHL4_GEOM_ID, 1 << 12);
        InitBreakable(this->wheelBreakableMgr, basename, "hub4", HUB4_GEOM_ID, 1 << 13);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr4", FNDR4_GEOM_ID, 1 << 14);
        InitBreakable(this->wheelBreakableMgr, basename, "whl5", WHL5_GEOM_ID, 1 << 15);
        InitBreakable(this->wheelBreakableMgr, basename, "hub5", HUB5_GEOM_ID, 1 << 16);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr5", FNDR5_GEOM_ID, 1 << 17);
        InitBreakable(this->wheelBreakableMgr, basename, "engine", ENGINE_GEOM_ID, 1 << 18);

        //load trailer hitch offset
        auto hitchGeom = this->GetGeom(3, TRAILER_HITCH_GEOM_ID);
        if (hitchGeom != nullptr)
        {
            Matrix34 outMatrix;
            GetPivot(outMatrix, basename, "trailer_hitch");
            this->trailerHitchPosition = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
        }
    }
        
    /*
        lvlInstance virtuals
    */

    AGE_API void vehCarModel::Reset()
    {
        ClearDamage();
    }

    AGE_API const Vector3 & vehCarModel::GetPosition()
                                                        { return hook::Thunk<0x4CEF50>::Call<const Vector3 &>(this); }
    AGE_API const Matrix34 & vehCarModel::GetMatrix(Matrix34 *a1)
                                                        { return hook::Thunk<0x4CEF90>::Call<const Matrix34 &>(this, a1); }
    AGE_API void vehCarModel::SetMatrix(const Matrix34 &a1)
                                                        { hook::Thunk<0x4CEFA0>::Call<void>(this, a1); }
    AGE_API dgPhysEntity * vehCarModel::GetEntity()         { return hook::Thunk<0x4CEFC0>::Call<dgPhysEntity*>(this); }
    AGE_API dgPhysEntity * vehCarModel::AttachEntity()
                                                        { return hook::Thunk<0x4CEFD0>::Call<dgPhysEntity*>(this); }
    AGE_API const Vector3 & vehCarModel::GetVelocity()
                                                        { return hook::Thunk<0x4CEF80>::Call<const Vector3 &>(this); }

    AGE_API void vehCarModel::Draw(int lod)
    {
        if (lod < 0 || lod > 3)
            return;
        if (this->GetGeomIndex() == 0)
            return;

        //disable alpha drawing
        bool alphaDrawing = false;

        //get shaders
        auto shaders = this->GetShader(this->GetVariant());

        //use texel damage
        if (lod >= 2 && this->texelDamage != nullptr)
        {
            shaders = this->texelDamage->CurrentShaders;
        }

        //use mm1 damage
        if (lod >= 2 && this->mm1Damage != nullptr)
        {
            shaders = this->mm1Damage->GetCleanShaders();
        }

        if (Enable3DDamage && mm1StyleDamage)
        {
            //use 3d damage
            if (lod >= 2 && this->damage3D != nullptr)
            {
                shaders = this->damage3D->GetCleanShaders();
            }
        }

        //setup renderer
        gfxRenderState::SetWorldMatrix(*this->carSim->GetWorldMatrix());

        //draw the body
        modStatic* bodyGeom = this->GetGeom(lod, 0);
        if (Enable3DDamage)
        {
            if (damage3D != nullptr)
            {
                bodyGeom = lod >= 2 ? damage3D->GetDeformModel() : this->GetGeom(lod, 0);
                if (bodyGeom != nullptr)
                    bodyGeom->DrawNoAlpha(shaders);

                if (mm1StyleDamage)
                {
                    modStatic* bodyDamage = lod >= 2 ? damage3D->GetDeformDamageModel() : this->GetGeom(lod, 0);
                    if (bodyDamage != nullptr)
                        bodyDamage->DrawNoAlpha(lod >= 2 ? damage3D->GetDamageShaders() : shaders);
                }
            }
        }
        else if (mm1StyleDamage)
        {
            if (mm1Damage != nullptr)
            {
                bodyGeom = lod >= 2 ? mm1Damage->GetCleanModel() : this->GetGeom(lod, 0);
                if (bodyGeom != nullptr)
                    bodyGeom->DrawNoAlpha(shaders);

                modStatic* bodyDamage = lod >= 2 ? mm1Damage->GetDamageModel() : this->GetGeom(lod, 0);
                if (bodyDamage != nullptr)
                    bodyDamage->DrawNoAlpha(lod >= 2 ? mm1Damage->GetDamageShaders() : shaders);
            }
        }
        else
        {
            if (bodyGeom != nullptr)
                bodyGeom->DrawNoAlpha(shaders);
        }

        //draw breakables
        this->genBreakableMgr->Draw(*this->carSim->GetWorldMatrix(), shaders, lod, alphaDrawing);

        //draw pop-up lights
        if (lod >= 1)
        {
            if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                //plighton
                DrawPart(lod, PLIGHTON_GEOM_ID, *this->carSim->GetWorldMatrix(), shaders, alphaDrawing);
            else
                //plightoff
                DrawPart(lod, PLIGHTOFF_GEOM_ID, *this->carSim->GetWorldMatrix(), shaders, alphaDrawing);
        }

        //get wheels
        auto carSim = this->carSim;
        vehWheel* wheels[6] = { carSim->GetWheel(0), carSim->GetWheel(1), carSim->GetWheel(2), carSim->GetWheel(3), carSim->GetWheel(2), carSim->GetWheel(3) };

        //get wheel, hub and fender ids
        int whlIds[6] = { WHL0_GEOM_ID, WHL1_GEOM_ID, WHL2_GEOM_ID, WHL3_GEOM_ID, WHL4_GEOM_ID, WHL5_GEOM_ID };
        int hubIds[6] = { HUB0_GEOM_ID, HUB1_GEOM_ID, HUB2_GEOM_ID, HUB3_GEOM_ID, HUB4_GEOM_ID, HUB5_GEOM_ID };
        int swhlIds[6] = { SWHL0_GEOM_ID, SWHL1_GEOM_ID, SWHL2_GEOM_ID, SWHL3_GEOM_ID, SWHL4_GEOM_ID, SWHL5_GEOM_ID };
        int shubIds[6] = { SHUB0_GEOM_ID, SHUB1_GEOM_ID, SHUB2_GEOM_ID, SHUB3_GEOM_ID, SHUB4_GEOM_ID, SHUB5_GEOM_ID };
        int fndrIds[6] = { FNDR0_GEOM_ID, FNDR1_GEOM_ID, FNDR2_GEOM_ID, FNDR3_GEOM_ID, FNDR4_GEOM_ID, FNDR5_GEOM_ID };

        //draw fenders
        if (lod == 3)
        {
            for (int i = 0; i < 6; i++)
            {
                int fndrId = fndrIds[i];
                int fndrStatusFlag = 1 << ((i * 3) + 2);
                auto fndrGeom = this->GetGeom(3, fndrId);
                if (fndrGeom != nullptr && (wheelBrokenStatus & fndrStatusFlag) != 0)
                {
                    DrawPart(lod, fndrId, this->GetFenderMatrix(i), shaders, alphaDrawing);
                }
            }
        }

        //draw wheels and hubs
        if (lod >= 1)
        {
            for (int i = 0; i < 6; i++)
            {
                auto wheel = wheels[i];

                //hub
                int hubStatusFlag = 1 << ((i * 3) + 1);
                if ((this->wheelBrokenStatus & hubStatusFlag) != 0)
                {
                    int shubId = shubIds[i];
                    int hubId = hubIds[i];
                    auto shubGeom = this->GetGeom(lod, shubId);
                    if (fabs(wheel->GetRotationRate()) >= BlurSpeed && shubGeom != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, shubId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                    }
                    else
                    {
                        DrawPart(lod, hubId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                    }
                }

                //whl
                int whlStatusFlag = 1 << (i * 3);
                if ((this->wheelBrokenStatus & whlStatusFlag) != 0)
                {
                    int swhlId = swhlIds[i];
                    int whlId = whlIds[i];
                    auto swhlGeom = this->GetGeom(lod, swhlId);
                    if (fabs(wheel->GetRotationRate()) >= BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, swhlId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                    }
                    else
                    {
                        DrawPart(lod, whlId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                    }
                }
            }
        }

        //draw suspension and engine
        if (lod >= 1)
        {
            //suspension
            for (int i = 0; i < 4; i++)
            {
                DrawPart(lod, SHOCK0_GEOM_ID + i, this->carSim->ShockSuspensions[i].getSuspensionMatrix(), shaders, alphaDrawing);
            }
            for (int i = 0; i < 4; i++)
            {
                DrawPart(lod, ARM0_GEOM_ID + i, this->carSim->ArmSuspensions[i].getSuspensionMatrix(), shaders, alphaDrawing);
            }
            for (int i = 0; i < 2; i++)
            {
                DrawPart(lod, SHAFT2_GEOM_ID + i, this->carSim->ShaftSuspensions[i].getSuspensionMatrix(), shaders, alphaDrawing);
            }

            DrawPart(lod, AXLE0_GEOM_ID, this->carSim->AxleFront.GetAxleMatrix(), shaders, alphaDrawing);
            DrawPart(lod, AXLE1_GEOM_ID, this->carSim->AxleRear.GetAxleMatrix(), shaders, alphaDrawing);
                
            //engine
            if ((this->wheelBrokenStatus & 0x40000) != 0)
            {
                auto engineMatrixPtr = this->carSim->GetEngine()->GetVisualMatrixPtr();
                if(engineMatrixPtr != nullptr)
                    DrawPart(lod, ENGINE_GEOM_ID, *engineMatrixPtr, shaders, alphaDrawing);
            }
        }
    }

    AGE_API void vehCarModel::DrawShadow()                  
    {
        if (vehCarModel::Enable3DShadows <= 1
            || MMSTATE->TimeOfDay == 3
            || lvlLevel::GetSingleton()->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
        {
            // draw drop shadow
            hook::Thunk<0x4CE940>::Call<void>(this);
            return;
        }

        auto timeWeather = cityLevel::GetCurrentLighting();

        bool prevLighting = gfxRenderState::SetLighting(true);

        auto prevCullMode = gfxRenderState::GetCullMode();
        gfxRenderState::SetCullMode(D3DCULL_CCW);

        //get shaders
        auto shaders = this->GetShader(this->GetVariant());

        //get model
        modStatic* model = this->GetGeomBase(0)->GetHighestLOD();

        if (model != nullptr)
        {
            Matrix34 shadowMatrix = Matrix34();
            Matrix34 carMatrix = *this->carSim->GetWorldMatrix();

            if (lvlInstance::ComputeShadowProjectionMatrix(shadowMatrix, this->GetRoomId(), timeWeather->KeyPitch, timeWeather->KeyHeading, carMatrix, this))
            {
                gfxRenderState::SetWorldMatrix(shadowMatrix);
                float intensity = ComputeShadowIntensity(timeWeather->KeyColor);
                model->DrawShadowed(shaders, intensity);

                //draw breakables
                this->genBreakableMgr->DrawShadowed(shadowMatrix, shaders, intensity);

                if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                    //plighton
                    DrawPartShadowed(3, PLIGHTON_GEOM_ID, shadowMatrix, shaders, intensity);
                else
                    //plightoff
                    DrawPartShadowed(3, PLIGHTOFF_GEOM_ID, shadowMatrix, shaders, intensity);

                //get wheels
                auto carSim = this->carSim;
                vehWheel* wheels[6] = { carSim->GetWheel(0), carSim->GetWheel(1), carSim->GetWheel(2), carSim->GetWheel(3), carSim->GetWheel(2), carSim->GetWheel(3) };

                //get wheel, hub and fender ids
                int whlIds[6] = { WHL0_GEOM_ID, WHL1_GEOM_ID, WHL2_GEOM_ID, WHL3_GEOM_ID, WHL4_GEOM_ID, WHL5_GEOM_ID };
                int hubIds[6] = { HUB0_GEOM_ID, HUB1_GEOM_ID, HUB2_GEOM_ID, HUB3_GEOM_ID, HUB4_GEOM_ID, HUB5_GEOM_ID };
                int swhlIds[6] = { SWHL0_GEOM_ID, SWHL1_GEOM_ID, SWHL2_GEOM_ID, SWHL3_GEOM_ID, SWHL4_GEOM_ID, SWHL5_GEOM_ID };
                int shubIds[6] = { SHUB0_GEOM_ID, SHUB1_GEOM_ID, SHUB2_GEOM_ID, SHUB3_GEOM_ID, SHUB4_GEOM_ID, SHUB5_GEOM_ID };
                int fndrIds[6] = { FNDR0_GEOM_ID, FNDR1_GEOM_ID, FNDR2_GEOM_ID, FNDR3_GEOM_ID, FNDR4_GEOM_ID, FNDR5_GEOM_ID };

                //draw wheels, hubs and fenders
                for (int i = 0; i < 6; i++)
                {
                    auto wheel = wheels[i];
                    int whlStatusFlag = 1 << (i * 3);
                    int hubStatusFlag = 1 << ((i * 3) + 1);
                    int fndrStatusFlag = 1 << ((i * 3) + 2);

                    //fndr
                    int fndrId = fndrIds[i];
                    auto fndrGeom = this->GetGeom(3, fndrId);
                    if (fndrGeom != nullptr && (wheelBrokenStatus & fndrStatusFlag) != 0)
                    {
                        DrawPartShadowed(3, fndrId, this->GetFenderShadowMatrix(i, shadowMatrix), shaders, intensity);
                    }

                    //hub
                    int hubId = hubIds[i];
                    auto hubGeom = this->GetGeom(3, hubId);
                    if (hubGeom != nullptr && (this->wheelBrokenStatus & hubStatusFlag) != 0)
                    {
                        int shubId = shubIds[i];
                        auto shubGeom = this->GetGeom(3, shubId);
                        if (fabs(wheel->GetRotationRate()) >= BlurSpeed && shubGeom != nullptr && vehCarModel::EnableSpinningWheels)
                        {
                            DrawPartShadowed(3, shubId, this->GetWheelShadowMatrix(i, shadowMatrix), shaders, intensity);
                        }
                        else
                        {
                            DrawPartShadowed(3, hubId, this->GetWheelShadowMatrix(i, shadowMatrix), shaders, intensity);
                        }
                    }

                    //wheel
                    int whlId = whlIds[i];
                    auto whlGeom = this->GetGeom(3, whlId);
                    if (whlGeom != nullptr && (this->wheelBrokenStatus & whlStatusFlag) != 0)
                    {
                        int swhlId = swhlIds[i];
                        auto swhlGeom = this->GetGeom(3, swhlId);
                        
                        if (fabs(wheel->GetRotationRate()) >= BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
                        {
                            DrawPartShadowed(3, swhlId, this->GetWheelShadowMatrix(i, shadowMatrix), shaders, intensity);
                        }
                        else
                        {
                            DrawPartShadowed(3, whlId, this->GetWheelShadowMatrix(i, shadowMatrix), shaders, intensity);
                        }
                    }
                }

                //arm
                for (int i = 0; i < 4; i++)
                {
                    int armGeomId = ARM0_GEOM_ID + i;
                    auto armGeom = this->GetGeom(3, armGeomId);
                    if (armGeom != nullptr)
                    {
                        Matrix34 armShadowMatrix = Matrix34();
                        armShadowMatrix.Identity();
                        armShadowMatrix.SetRow(3, this->carSim->ArmSuspensions[i].getSuspensionPivot().GetRow(3));
                        armShadowMatrix.RotateY((i % 2 == 0) ? 1.570796f : -1.570796f); //rotate arm 90 degrees if id is even, if it's odd then rotate -90 degrees
                        armShadowMatrix.ScaleFull(1.0f - this->carSim->GetWheel(i)->GetBumpDisplacement(), 1.0f, 1.0f);
                        armShadowMatrix.Dot(shadowMatrix);
                        DrawPartShadowed(3, armGeomId, armShadowMatrix, shaders, intensity);
                    }
                }

                //engine
                auto engineGeom = this->GetGeom(3, ENGINE_GEOM_ID);
                if (engineGeom != nullptr && (this->wheelBrokenStatus & 0x40000) != 0)
                {
                    auto engineMatrixPtr = this->carSim->GetEngine()->GetVisualMatrixPtr();
                    if (engineMatrixPtr != nullptr)
                    {
                        Matrix34 engineShadowMatrix = Matrix34();
                        if (lvlInstance::ComputeShadowProjectionMatrix(engineShadowMatrix, this->GetRoomId(), timeWeather->KeyPitch, timeWeather->KeyHeading, *engineMatrixPtr, this))
                        {
                            DrawPartShadowed(3, ENGINE_GEOM_ID, engineShadowMatrix, shaders, intensity);
                        }
                    }
                }
            }
        }

        //revert model faces back
        gfxRenderState::SetCullMode(prevCullMode);
        gfxRenderState::SetLighting(prevLighting);
    }

    AGE_API void vehCarModel::DrawShadowMap()               { hook::Thunk<0x4CEA90>::Call<void>(this); }
        
    AGE_API void vehCarModel::DrawGlow()
    {
        if (!this->GetVisible())
            return;
        if (this->GetGeomIndex() == 0)
            return;

        //get shaders
        auto shaders = this->GetShader(this->GetVariant());

        //get car stuff we use to determine what to darw
        auto car = this->GetCar();
        auto carsim = this->carSim;
        auto siren = car->GetSiren();
        auto curDamage = car->GetCarDamage()->GetCurDamage();
        auto maxDamage = car->GetCarDamage()->GetMaxDamage();
        int gear = carsim->GetTransmission()->GetGear();
        if (curDamage >= maxDamage && vehCarModel::MWStyleTotaledCar)
            return;

        //setup renderer
        gfxRenderState::SetWorldMatrix(*this->carSim->GetWorldMatrix());

        //draw signals
        modStatic* slight0 = this->GetGeomBase(SLIGHT0_GEOM_ID)->GetHighestLOD();
        modStatic* slight1 = this->GetGeomBase(SLIGHT1_GEOM_ID)->GetHighestLOD();

        //draw brake signals
        modStatic* tslight0 = this->GetGeomBase(TSLIGHT0_GEOM_ID)->GetHighestLOD();
        modStatic* tslight1 = this->GetGeomBase(TSLIGHT1_GEOM_ID)->GetHighestLOD();

        //check signal clock
        bool drawSignal = fmodf(datTimeManager::ElapsedTime + (float)this->GetRandId(), 1.0f) > 0.5f;

        //draw stuff!
        if (drawSignal && car->IsPlayer())
        {
            if (LeftSignalLightState || HazardLightsState)
            {
                if (slight0 != nullptr)
                    slight0->Draw(shaders);
                if (tslight0 != nullptr)
                    tslight0->Draw(shaders);
            }
            if (RightSignalLightState || HazardLightsState)
            {
                if (slight1 != nullptr)
                    slight1->Draw(shaders);
                if (tslight1 != nullptr)
                    tslight1->Draw(shaders);
            }
        }

        //draw brake signals for player
        if (car->IsPlayer()) {
            if (!LeftSignalLightState && !HazardLightsState)
            {
                if (tslight0 != nullptr)
                {
                    //draw brake copy
                    if (carsim->GetBrake() > 0.1)
                        tslight0->Draw(shaders);
                    //draw headlight copy
                    if (vehCarModel::ShowHeadlights)
                        tslight0->Draw(shaders);
                }
            }
            if (!RightSignalLightState && !HazardLightsState)
            {
                if (tslight1 != nullptr)
                {
                    //draw brake copy
                    if (carsim->GetBrake() > 0.1)
                        tslight1->Draw(shaders);
                    //draw headlight copy
                    if (vehCarModel::ShowHeadlights)
                        tslight1->Draw(shaders);
                }
            }
        }

        //draw brake signals for cops and opponents
        if (!car->IsPlayer())
        {
            if (tslight0 != nullptr)
            {
                //draw brake copy
                if (carsim->GetBrake() > 0.1)
                    tslight0->Draw(shaders);
                //draw headlight copy
                if (vehCar::sm_DrawHeadlights)
                    tslight0->Draw(shaders);
            }
            if (tslight1 != nullptr) {
                //draw brake copy
                if (carsim->GetBrake() > 0.1)
                    tslight1->Draw(shaders);
                //draw headlight copy
                if (vehCar::sm_DrawHeadlights)
                    tslight1->Draw(shaders);
            }
        }

        //only draw rear lights if the electrics allow it
        if (enabledElectrics[0] || enabledElectrics[1])
        {   
            //draw tlight
            modStatic* tlight = this->GetGeomBase(TLIGHT_GEOM_ID)->GetHighestLOD();
            if (tlight != nullptr)
            {
                //draw brake copy
                if (carsim->GetBrake() > 0.1)
                    tlight->Draw(shaders);
                //draw headlight copy
                if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                    tlight->Draw(shaders);
            }

            //draw blight
            modStatic* blight = this->GetGeomBase(BLIGHT_GEOM_ID)->GetHighestLOD();
            if (blight != nullptr)
            {
                //draw brake copy
                if (carsim->GetBrake() > 0.1)
                    blight->Draw(shaders);
            }

            //draw rlight
            modStatic* rlight = this->GetGeomBase(RLIGHT_GEOM_ID)->GetHighestLOD();
            if (mm1StyleTransmission)
            {
                auto throttle = carsim->GetEngine()->GetThrottleInput();
                auto speedMPH = carsim->GetSpeedMPH();
                auto transmission = carsim->GetTransmission();

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
                if (rlight != nullptr && gear == 0)
                {
                    rlight->Draw(shaders);
                }
            }
        }

        //Draw siren and headlights
        modStatic* hlight = this->GetGeomBase(HLIGHT_GEOM_ID)->GetHighestLOD();
        modStatic* siren0 = this->GetGeomBase(SIREN0_GEOM_ID)->GetHighestLOD();
        modStatic* siren1 = this->GetGeomBase(SIREN1_GEOM_ID)->GetHighestLOD();

        if (vehCarModel::HeadlightType < 3)
        {
            if (vehCarModel::HeadlightType == 0 || vehCarModel::HeadlightType == 2)
            {
                //MM2 headlights
                if (vehCarModel::EnableHeadlightFlashing)
                {
                    if (siren != nullptr && siren->IsActive())
                    {
                        this->DrawHeadlights(true);
                        this->DrawExtraHeadlights(true);
                    }
                    else if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                    {
                        this->DrawHeadlights(false);
                        this->DrawExtraHeadlights(false);
                    }
                }
                else
                {
                    if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                    {
                        this->DrawHeadlights(false);
                        this->DrawExtraHeadlights(false);
                    }
                }
            }
            if (vehCarModel::HeadlightType == 1 || vehCarModel::HeadlightType == 2)
            {
                //MM1 headlights
                gfxRenderState::SetWorldMatrix(*this->carSim->GetWorldMatrix());

                if (enabledElectrics[2] || enabledElectrics[3])
                {
                    if (hlight != nullptr)
                    {
                        if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                            hlight->Draw(shaders);
                    }
                }
            }
        }

        if (vehCarModel::SirenType < 3)
        {
            if (vehCarModel::SirenType == 0 || vehCarModel::SirenType == 2)
            {
                //MM2 siren
                if (siren != nullptr && siren->IsActive())
                    siren->Draw(*this->carSim->GetWorldMatrix());
            }
            if (vehCarModel::SirenType == 1 || vehCarModel::SirenType == 2)
            {
                //MM1 siren
                gfxRenderState::SetWorldMatrix(*this->carSim->GetWorldMatrix());

                if (siren != nullptr && siren->IsActive())
                {
                    int sirenStage = fmodf(datTimeManager::ElapsedTime + (float)this->GetRandId(), 2 * vehCarModel::SirenCycle) >= vehCarModel::SirenCycle ? 1 : 0;
                    if (sirenStage == 0 && siren0 != nullptr)
                        siren0->Draw(shaders);

                    else if (sirenStage == 1 && siren1 != nullptr)
                        siren1->Draw(shaders);
                }
            }
        }
    }

    AGE_API void vehCarModel::DrawReflected(float intensity)
    {
        if (this->GetGeomIndex() == 0)
            return;

        //get shaders
        auto shaders = this->GetShader(this->GetVariant());

        //use texel damage
        if (this->texelDamage != nullptr)
        {
            shaders = this->texelDamage->CurrentShaders;
        }

        //use mm1 damage
        if (this->mm1Damage != nullptr)
        {
            shaders = this->mm1Damage->GetCleanShaders();
        }

        if (Enable3DDamage && mm1StyleDamage)
        {
            //use 3d damage
            if (this->damage3D != nullptr)
            {
                shaders = this->damage3D->GetCleanShaders();
            }
        }

        //setup renderer
        gfxRenderState::SetWorldMatrix(*this->carSim->GetWorldMatrix());

        //draw the body
        modStatic* bodyGeom = this->GetGeom(3, 0);
        if (Enable3DDamage)
        {
            if (damage3D != nullptr)
            {
                bodyGeom = damage3D->GetDeformModel();

                if (mm1StyleDamage && DamageReflections)
                {
                    modStatic* bodyDamage = damage3D->GetDeformDamageModel();
                    if (bodyDamage != nullptr)
                        bodyDamage->DrawReflected(shaders, this->GetRoomId(), *this->carSim->GetWorldMatrix(), intensity);
                }
            }
        }
        else if (mm1StyleDamage)
        {
            if (mm1Damage != nullptr)
            {
                bodyGeom = mm1Damage->GetCleanModel();

                if (DamageReflections)
                {
                    modStatic* bodyDamage = mm1Damage->GetDamageModel();
                    if (bodyDamage != nullptr)
                        bodyDamage->DrawReflected(shaders, this->GetRoomId(), *this->carSim->GetWorldMatrix(), intensity);
                }
            }
        }

        if (bodyGeom != nullptr)
            bodyGeom->DrawReflected(shaders, this->GetRoomId(), *this->carSim->GetWorldMatrix(), intensity);

        //draw breakables
        this->genBreakableMgr->DrawReflected(*this->carSim->GetWorldMatrix(), shaders, 3, this->GetRoomId());

        //draw pop-up lights
        if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
            //plighton
            DrawPartReflected(3, PLIGHTON_GEOM_ID, *this->carSim->GetWorldMatrix(), shaders, intensity, vehCarModel::PartReflections);
        else
            //plightoff
            DrawPartReflected(3, PLIGHTOFF_GEOM_ID, *this->carSim->GetWorldMatrix(), shaders, intensity, vehCarModel::PartReflections);

        //get wheels
        auto carSim = this->carSim;
        vehWheel* wheels[6] = { carSim->GetWheel(0), carSim->GetWheel(1), carSim->GetWheel(2), carSim->GetWheel(3), carSim->GetWheel(2), carSim->GetWheel(3) };

        //get wheel, hub and fender ids
        int whlIds[6] = { WHL0_GEOM_ID, WHL1_GEOM_ID, WHL2_GEOM_ID, WHL3_GEOM_ID, WHL4_GEOM_ID, WHL5_GEOM_ID };
        int hubIds[6] = { HUB0_GEOM_ID, HUB1_GEOM_ID, HUB2_GEOM_ID, HUB3_GEOM_ID, HUB4_GEOM_ID, HUB5_GEOM_ID };
        int swhlIds[6] = { SWHL0_GEOM_ID, SWHL1_GEOM_ID, SWHL2_GEOM_ID, SWHL3_GEOM_ID, SWHL4_GEOM_ID, SWHL5_GEOM_ID };
        int shubIds[6] = { SHUB0_GEOM_ID, SHUB1_GEOM_ID, SHUB2_GEOM_ID, SHUB3_GEOM_ID, SHUB4_GEOM_ID, SHUB5_GEOM_ID };
        int fndrIds[6] = { FNDR0_GEOM_ID, FNDR1_GEOM_ID, FNDR2_GEOM_ID, FNDR3_GEOM_ID, FNDR4_GEOM_ID, FNDR5_GEOM_ID };

        //draw fenders
        for (int i = 0; i < 6; i++)
        {
            int fndrId = fndrIds[i];
            int fndrStatusFlag = 1 << ((i * 3) + 2);
            auto fndrGeom = this->GetGeom(3, fndrId);
            if (fndrGeom != nullptr && (wheelBrokenStatus & fndrStatusFlag) != 0)
            {
                DrawPartReflected(3, fndrId, this->GetFenderMatrix(i), shaders, intensity, vehCarModel::PartReflections);
            }
        }

        //draw wheels and hubs
        for (int i = 0; i < 6; i++)
        {
            auto wheel = wheels[i];

            //hub
            int hubStatusFlag = 1 << ((i * 3) + 1);
            if ((this->wheelBrokenStatus & hubStatusFlag) != 0)
            {
                int shubId = shubIds[i];
                int hubId = hubIds[i];
                auto shubGeom = this->GetGeom(3, shubId);
                if (fabs(wheel->GetRotationRate()) >= BlurSpeed && shubGeom != nullptr && vehCarModel::EnableSpinningWheels)
                {
                    DrawPartReflected(3, shubId, this->GetWheelMatrix(i), shaders, intensity, vehCarModel::WheelReflections);
                }
                else
                {
                    DrawPartReflected(3, hubId, this->GetWheelMatrix(i), shaders, intensity, vehCarModel::WheelReflections);
                }
            }

            //whl
            int whlStatusFlag = 1 << (i * 3);
            if ((this->wheelBrokenStatus & whlStatusFlag) != 0)
            {
                int swhlId = swhlIds[i];
                int whlId = whlIds[i];
                auto swhlGeom = this->GetGeom(3, swhlId);
                if (fabs(wheel->GetRotationRate()) >= BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
                {
                    DrawPartReflected(3, swhlId, this->GetWheelMatrix(i), shaders, intensity, vehCarModel::WheelReflections);
                }
                else
                {
                    DrawPartReflected(3, whlId, this->GetWheelMatrix(i), shaders, intensity, vehCarModel::WheelReflections);
                }
            }
        }

        //draw suspension and engine

        //suspension
        for (int i = 0; i < 4; i++)
        {
            DrawPartReflected(3, SHOCK0_GEOM_ID + i, this->carSim->ShockSuspensions[i].getSuspensionMatrix(), shaders, intensity, vehCarModel::PartReflections);
        }
        for (int i = 0; i < 4; i++)
        {
            DrawPartReflected(3, ARM0_GEOM_ID + i, this->carSim->ArmSuspensions[i].getSuspensionMatrix(), shaders, intensity, vehCarModel::PartReflections);
        }
        for (int i = 0; i < 2; i++)
        {
            DrawPartReflected(3, SHAFT2_GEOM_ID + i, this->carSim->ShaftSuspensions[i].getSuspensionMatrix(), shaders, intensity, vehCarModel::PartReflections);
        }

        DrawPartReflected(3, AXLE0_GEOM_ID, this->carSim->AxleFront.GetAxleMatrix(), shaders, intensity, vehCarModel::PartReflections);
        DrawPartReflected(3, AXLE1_GEOM_ID, this->carSim->AxleRear.GetAxleMatrix(), shaders, intensity, vehCarModel::PartReflections);

        //engine
        if ((this->wheelBrokenStatus & 0x40000) != 0)
        {
            auto engineMatrixPtr = this->carSim->GetEngine()->GetVisualMatrixPtr();
            if (engineMatrixPtr != nullptr)
                DrawPartReflected(3, ENGINE_GEOM_ID, *engineMatrixPtr, shaders, intensity, vehCarModel::PartReflections);
        }
    }

    AGE_API void vehCarModel::DrawReflectedParts(int lod)
    {
        if (lod < 0 || lod > 3)
            return;
        if (this->GetGeomIndex() == 0)
            return;

        //enable alpha drawing
        bool alphaDrawing = true;

        //get shaders
        auto shaders = this->GetShader(this->GetVariant());

        //use texel damage
        if (lod >= 2 && this->texelDamage != nullptr)
        {
            shaders = this->texelDamage->CurrentShaders;
        }

        //use mm1 damage
        if (lod >= 2 && this->mm1Damage != nullptr)
        {
            shaders = this->mm1Damage->GetCleanShaders();
        }

        if (Enable3DDamage && mm1StyleDamage)
        {
            //use 3d damage
            if (lod >= 2 && this->damage3D != nullptr)
            {
                shaders = this->damage3D->GetCleanShaders();
            }
        }

        //vppanozgt hack... use full alpha and disable ZWrite for paintjob 4
        int prevAlphaRef = gfxRenderState::GetAlphaRef();
        int prevZWriteEnable = gfxRenderState::GetZWriteEnabled();

        if (this->GetVariant() == 4 && !strcmp(this->GetName(), "vppanozgt_body"))
        {
            gfxRenderState::SetAlphaRef(0);
            gfxRenderState::SetZWriteEnabled(false);
        }

        //setup renderer
        gfxRenderState::SetWorldMatrix(*this->carSim->GetWorldMatrix());

        //draw the body
        modStatic* bodyGeom = this->GetGeom(lod, 0);
        if (Enable3DDamage)
        {
            if (damage3D != nullptr)
            {
                bodyGeom = lod >= 2 ? damage3D->GetDeformModel() : this->GetGeom(lod, 0);
                if (bodyGeom != nullptr)
                    bodyGeom->DrawAlpha(shaders);

                if (mm1StyleDamage)
                {
                    modStatic* bodyDamage = lod >= 2 ? damage3D->GetDeformDamageModel() : this->GetGeom(lod, 0);
                    if (bodyDamage != nullptr)
                        bodyDamage->DrawAlpha(lod >= 2 ? damage3D->GetDamageShaders() : shaders);
                }
            }
        }
        else if (mm1StyleDamage)
        {
            if (mm1Damage != nullptr)
            {
                bodyGeom = lod >= 2 ? mm1Damage->GetCleanModel() : this->GetGeom(lod, 0);
                if (bodyGeom != nullptr)
                    bodyGeom->DrawAlpha(shaders);

                modStatic* bodyDamage = lod >= 2 ? mm1Damage->GetDamageModel() : this->GetGeom(lod, 0);
                if (bodyDamage != nullptr)
                    bodyDamage->DrawAlpha(lod >= 2 ? mm1Damage->GetDamageShaders() : shaders);
            }
        }
        else
        {
            if (bodyGeom != nullptr)
                bodyGeom->DrawAlpha(shaders);
        }

        //draw breakables
        this->genBreakableMgr->Draw(*this->carSim->GetWorldMatrix(), shaders, lod, alphaDrawing);

        //draw decal
        auto decalGeom = this->GetGeom(lod, DECAL_GEOM_ID);
        if (decalGeom != nullptr)
        {
            auto oldAlphaRef2 = gfxRenderState::SetAlphaRef(0);
            DrawPart(decalGeom, *this->carSim->GetWorldMatrix(), shaders);
            gfxRenderState::SetAlphaRef(oldAlphaRef2);
        }

        //draw pop-up lights
        if (lod >= 1)
        {
            if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                //plighton
                DrawPart(lod, PLIGHTON_GEOM_ID, *this->carSim->GetWorldMatrix(), shaders, alphaDrawing);
            else
                //plightoff
                DrawPart(lod, PLIGHTOFF_GEOM_ID, *this->carSim->GetWorldMatrix(), shaders, alphaDrawing);
        }

        //get wheels
        auto carSim = this->carSim;
        vehWheel* wheels[6] = { carSim->GetWheel(0), carSim->GetWheel(1), carSim->GetWheel(2), carSim->GetWheel(3), carSim->GetWheel(2), carSim->GetWheel(3) };

        //get wheel, hub and fender ids
        int whlIds[6] = { WHL0_GEOM_ID, WHL1_GEOM_ID, WHL2_GEOM_ID, WHL3_GEOM_ID, WHL4_GEOM_ID, WHL5_GEOM_ID };
        int hubIds[6] = { HUB0_GEOM_ID, HUB1_GEOM_ID, HUB2_GEOM_ID, HUB3_GEOM_ID, HUB4_GEOM_ID, HUB5_GEOM_ID };
        int swhlIds[6] = { SWHL0_GEOM_ID, SWHL1_GEOM_ID, SWHL2_GEOM_ID, SWHL3_GEOM_ID, SWHL4_GEOM_ID, SWHL5_GEOM_ID };
        int shubIds[6] = { SHUB0_GEOM_ID, SHUB1_GEOM_ID, SHUB2_GEOM_ID, SHUB3_GEOM_ID, SHUB4_GEOM_ID, SHUB5_GEOM_ID };
        int fndrIds[6] = { FNDR0_GEOM_ID, FNDR1_GEOM_ID, FNDR2_GEOM_ID, FNDR3_GEOM_ID, FNDR4_GEOM_ID, FNDR5_GEOM_ID };

        //draw fenders
        if (lod == 3)
        {
            for (int i = 0; i < 6; i++)
            {
                int fndrId = fndrIds[i];
                int fndrStatusFlag = 1 << ((i * 3) + 2);
                auto fndrGeom = this->GetGeom(3, fndrId);
                if (fndrGeom != nullptr && (wheelBrokenStatus & fndrStatusFlag) != 0)
                {
                    DrawPart(lod, fndrId, this->GetFenderMatrix(i), shaders, alphaDrawing);
                }
            }
        }

        //draw wheels and hubs
        if (lod >= 1)
        {
            for (int i = 0; i < 6; i++)
            {
                auto wheel = wheels[i];

                //hub
                int hubStatusFlag = 1 << ((i * 3) + 1);
                if ((this->wheelBrokenStatus & hubStatusFlag) != 0)
                {
                    int shubId = shubIds[i];
                    int hubId = hubIds[i];
                    auto shubGeom = this->GetGeom(lod, shubId);
                    if (fabs(wheel->GetRotationRate()) >= BlurSpeed && shubGeom != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, shubId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                    }
                    else
                    {
                        DrawPart(lod, hubId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                    }
                }

                //whl
                int whlStatusFlag = 1 << (i * 3);
                if ((this->wheelBrokenStatus & whlStatusFlag) != 0)
                {
                    int swhlId = swhlIds[i];
                    int whlId = whlIds[i];
                    auto swhlGeom = this->GetGeom(lod, swhlId);
                    if (fabs(wheel->GetRotationRate()) >= BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, swhlId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                    }
                    else
                    {
                        DrawPart(lod, whlId, this->GetWheelMatrix(i), shaders, alphaDrawing);
                    }
                }
            }
        }

        //draw suspension and engine
        if (lod >= 1)
        {
            //suspension
            for (int i = 0; i < 4; i++)
            {
                DrawPart(lod, SHOCK0_GEOM_ID + i, this->carSim->ShockSuspensions[i].getSuspensionMatrix(), shaders, alphaDrawing);
            }
            for (int i = 0; i < 4; i++)
            {
                DrawPart(lod, ARM0_GEOM_ID + i, this->carSim->ArmSuspensions[i].getSuspensionMatrix(), shaders, alphaDrawing);
            }
            for (int i = 0; i < 2; i++)
            {
                DrawPart(lod, SHAFT2_GEOM_ID + i, this->carSim->ShaftSuspensions[i].getSuspensionMatrix(), shaders, alphaDrawing);
            }

            DrawPart(lod, AXLE0_GEOM_ID, this->carSim->AxleFront.GetAxleMatrix(), shaders, alphaDrawing);
            DrawPart(lod, AXLE1_GEOM_ID, this->carSim->AxleRear.GetAxleMatrix(), shaders, alphaDrawing);

            //engine
            if ((this->wheelBrokenStatus & 0x40000) != 0)
            {
                auto engineMatrixPtr = this->carSim->GetEngine()->GetVisualMatrixPtr();
                if (engineMatrixPtr != nullptr)
                    DrawPart(lod, ENGINE_GEOM_ID, *engineMatrixPtr, shaders, alphaDrawing);
            }
        }

        //set ZWrite back
        gfxRenderState::SetZWriteEnabled(prevZWriteEnable);

        //set alpha back if required
        gfxRenderState::SetAlphaRef(prevAlphaRef);
    }

    AGE_API unsigned int vehCarModel::SizeOf()              { return sizeof(vehCarModel); }



    void vehCarModel::BindLua(LuaState L) {
        LuaBinding(L).beginExtendClass<vehCarModel, lvlInstance>("vehCarModel")
            //properties
            .addPropertyReadOnly("Breakables", &GetGenBreakableMgr)
            .addPropertyReadOnly("WheelBreakables", &GetMechBreakableMgr)
            .addPropertyReadOnly("TrailerHitchOffset", &GetTrailerHitchOffset)
            .addPropertyReadOnly("RandId", &GetRandId)
            .addProperty("Variant", &GetVariant, &SetVariant)
            .addProperty("Visible", &GetVisible, &SetVisible)
                
            .addStaticVariableRef("ShowHeadlights", &vehCarModel::ShowHeadlights)
            .addStaticVariableRef("LeftSignalLightState", &vehCarModel::LeftSignalLightState)
            .addStaticVariableRef("RightSignalLightState", &vehCarModel::RightSignalLightState)
            .addStaticVariableRef("HazardLightsState", &vehCarModel::HazardLightsState)
                
            //lua functions
                

            //functions
            .addFunction("Reset", &Reset)
            .addFunction("BreakElectrics", &BreakElectrics)
            .addFunction("ClearDamage", &ClearDamage)
            .addFunction("EjectOneshot", &EjectOneshot)

            .addFunction("GetHeadlight", &GetHeadlight)


        .endClass();
    }

}