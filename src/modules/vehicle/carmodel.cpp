#pragma once
#include "carmodel.h"
#include <modules\city\citylevel.h>
#include <modules\mmcityinfo\state.h>

namespace MM2
{
    const float BlurSpeed = 26.0f;

    AGE_API vehCarModel::vehCarModel()
    {
        scoped_vtable x(this);
        hook::Thunk<0x4CCF20>::Call<void>(this);
    }

    AGE_API vehCarModel::~vehCarModel()
    {
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

    int vehCarModel::GetVariant() const
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
                texelDamage->Init(bodyEntry->GetHighLOD(), bodyEntry->pShaders[variant], bodyEntry->numShaders);
            }
        }
        if (mm1Damage)
        {
            auto bodyEntry = this->GetGeomBase();
            if (bodyEntry->GetHighLOD() != nullptr)
            {
                delete mm1Damage;
                mm1Damage = new mmDamage();
                mm1Damage->Init(bodyEntry->GetHighLOD(), bodyEntry->pShaders[variant], bodyEntry->numShaders);
            }
        }
        if (damage3D)
        {
            auto bodyEntry = this->GetGeomBase();
            modStatic* bodyDamageEntry = this->GetGeomEntry(this->GetGeomId("bodydamage"));
            if (bodyEntry->GetHighLOD() != nullptr)
            {
                delete damage3D;
                damage3D = new fxDamage3D();
                damage3D->Init(bodyEntry->GetHighLOD(), bodyDamageEntry != nullptr ? bodyDamageEntry : bodyEntry->GetHighLOD(), vehCarModel::MM1StyleDamage);
                damage3D->SetShaders(bodyEntry->pShaders[variant], bodyEntry->numShaders);
            }
        }
        if (lightDamage)
        {
            delete lightDamage;
            lightDamage = new mmLightDamage();
            modStatic* hlight = this->GetGeomEntry(this->GetGeomId("hlight")); if (hlight != nullptr) lightDamage->Init(hlight, mmLightDamage::HLIGHT);
            modStatic* tlight = this->GetGeomEntry(this->GetGeomId("tlight")); if (tlight != nullptr) lightDamage->Init(tlight, mmLightDamage::TLIGHT);
            modStatic* rlight = this->GetGeomEntry(this->GetGeomId("rlight")); if (rlight != nullptr) lightDamage->Init(rlight, mmLightDamage::RLIGHT);
            modStatic* slight0 = this->GetGeomEntry(this->GetGeomId("slight0")); if (slight0 != nullptr) lightDamage->Init(slight0, mmLightDamage::SLIGHT0);
            modStatic* slight1 = this->GetGeomEntry(this->GetGeomId("slight1")); if (slight1 != nullptr) lightDamage->Init(slight1, mmLightDamage::SLIGHT1);
            modStatic* blight = this->GetGeomEntry(this->GetGeomId("blight")); if (blight != nullptr) lightDamage->Init(blight, mmLightDamage::BLIGHT);
            modStatic* siren0 = this->GetGeomEntry(this->GetGeomId("siren0")); if (siren0 != nullptr) lightDamage->Init(siren0, mmLightDamage::SIREN0);
            modStatic* siren1 = this->GetGeomEntry(this->GetGeomId("siren1")); if (siren1 != nullptr) lightDamage->Init(siren1, mmLightDamage::SIREN1);
            modStatic* tslight0 = this->GetGeomEntry(this->GetGeomId("tslight0")); if (tslight0 != nullptr) lightDamage->Init(tslight0, mmLightDamage::TSLIGHT0);
            modStatic* tslight1 = this->GetGeomEntry(this->GetGeomId("tslight1")); if (tslight1 != nullptr) lightDamage->Init(tslight1, mmLightDamage::TSLIGHT1);
        }
    }

    ltLight* vehCarModel::GetHeadlight(int index)
    {
        if (this->headlights == nullptr)
            return nullptr;
        return &this->headlights[index];
    }

    Vector3 vehCarModel::GetHeadlightPosition(int index)
    {
        if (this->headlightPositions == nullptr)
            return Vector3::ORIGIN;
		return this->headlightPositions[index];
	}

	bool vehCarModel::GetHeadlightElectric(int index)
    {
        if (this->headlightElectrics == nullptr)
            return false;
        return this->headlightElectrics[index];
	}

    int vehCarModel::GetWheelBrokenStatus() const
    {
        return this->wheelBrokenStatus;
    }

    Vector3 vehCarModel::GetTrailerHitchOffset() const
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

    mmLightDamage* vehCarModel::GetLightDamage()
    {
        return lightDamage;
    }

    bool vehCarModel::GetPrevSplashState() const
    {
        return prevSplashState;
    }

    void vehCarModel::SetPrevSplashState(bool state)
    {
        prevSplashState = state;
    }

    bool vehCarModel::GetCustomGlowState() const
    {
        return customGlowState;
    }

    void vehCarModel::SetCustomGlowState(bool state)
    {
        customGlowState = state;
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
            Vector3 whlPosDiff[2] = { this->carSim->BackBackLeftWheelPosDiff, this->carSim->BackBackRightWheelPosDiff };
            whlMatrix.Set(this->carSim->GetWheel(num - 2)->GetMatrix());
            whlMatrix.SetRow(3, whlMatrix.GetRow(3) + this->GetWorldMatrix().Transform3x3(whlPosDiff[num - 4]));
        }

        return whlMatrix;
    }

    Matrix34 vehCarModel::GetFenderMatrix(int num)
    {
        Matrix34 fndrMatrix = Matrix34();
        fndrMatrix.Identity();

        Matrix34 carMatrix = this->GetWorldMatrix();
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
            whlShadowMatrix.SetRow(3, wheel->GetCenter() + extraWhlPosDiff[num - 4]);
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

    float vehCarModel::GetWobbleAngle(vehWheel* wheel, float wobbleMultiplier)
    {
        float rotation = (fabsf(wheel->GetRotationRate()) * datTimeManager::Seconds - 1.5707964f) * 0.31830987f;
        float rotationAmount = fmaxf(0.0f, fminf(rotation, 1.0f));
        float invRotationAmount = 1.0f - rotationAmount;

        auto carDamage = this->car->GetCarDamage();
        float damagePercent = (carDamage->GetCurDamage() - carDamage->GetMedDamage()) / (carDamage->GetMaxDamage() - carDamage->GetMedDamage());
        float damageAmount = fmaxf(0.0f, fminf(damagePercent, 1.0f));
        float angle = sinf(wheel->GetAccumulatedRotation()) * damageAmount * invRotationAmount * wobbleMultiplier;

        if (!vehCarModel::MM1StyleWobble)
            angle = sinf(wheel->GetAccumulatedRotation()) * damageAmount * wheel->GetWobbleAmount() * 0.5f * (wobbleMultiplier / 0.1f);

        return angle;
    }

    // We created this matrix to draw car body with wobble angling
    Matrix34 vehCarModel::GetBodyWobbleMatrix(const Matrix34& matrix, vehWheel* wheel, float wobbleMultiplier)
    {
        if (!vehCarModel::EnableWheelWobble || !this->carSim->OnGround())
            return matrix;

        Matrix34 wobbleMatrix = Matrix34();
        wobbleMatrix.Set(matrix);

        Vector3 axis = wobbleMatrix.GetRow(2) + wobbleMatrix.GetRow(0);
        wobbleMatrix.Rotate(axis, GetWobbleAngle(wheel, wobbleMultiplier));

        return wobbleMatrix;
    }

    Matrix34 vehCarModel::GetPartWobbleMatrix(const Matrix34& matrix, vehWheel* wheel, float wobbleMultiplier)
    {
        if (!vehCarModel::EnableWheelWobble)
            return matrix;

        Matrix34 wobbleMatrix = Matrix34();
        wobbleMatrix.Set(matrix);

        Vector3 axis = wobbleMatrix.GetRow(2) + wobbleMatrix.GetRow(0);
        wobbleMatrix.Dot(GetWorldMatrix());
        wobbleMatrix.Rotate(axis, GetWobbleAngle(wheel, wobbleMultiplier));

        return wobbleMatrix;
    }

    Matrix34 vehCarModel::GetWorldMatrix()
    {
        return GetBodyWobbleMatrix(*this->carSim->GetWorldMatrix(), this->carSim->GetWheel(1));
    }

    Vector3 vehCarModel::getSurfaceColorLua(modStatic* model)
    {
        Vector3 outColor = Vector3::ORIGIN;
        GetSurfaceColor(model, outColor);
        return outColor;
	}

    AGE_API void vehCarModel::GetSurfaceColor(modStatic* model, Vector3& outColor)
                                                        { hook::Thunk<0x4CDF00>::Call<void>(this, model, &outColor); }
    AGE_API void vehCarModel::InitBreakable(vehBreakableMgr* manager, const char* basename, const char* breakableName, int geomId, int index)
                                                        { hook::Thunk<0x4CDC50>::Call<void>(this, manager, basename, breakableName, geomId, index); }

    void vehCarModel::InitBreakable(vehBreakableMgr* manager, const char* basename, const char* breakableName, int index)
    {
        int geomId = this->GetGeomId(breakableName);
        if (geomId < 0)
            return;

        InitBreakable(manager, basename, breakableName, geomId, index);
    }

    AGE_API void vehCarModel::InitSirenLight(const char* basename, const char* mtxName, int geomId)
    {
        Vector3 outColor = Vector3::ORIGIN;
        auto sirenGeom = this->GetGeom(3, geomId);
        if (sirenGeom != nullptr)
        {
            auto siren = this->car->GetSiren();
            Matrix34 outMatrix;

            GetPivot(outMatrix, basename, mtxName);
            this->GetSurfaceColor(sirenGeom, outColor);
            siren->AddLight(outMatrix.GetRow(3), outColor);
        }    
    }

    void vehCarModel::InitSirenLights(const char* basename)
    {
        auto siren = this->car->GetSiren();
        if (siren != nullptr)
        {
            int lightCount = this->GetGeomCount("srn");
            siren->Init(lightCount);
            siren->RemoveAllLights();
            for (int i = 0; i < lightCount; i++)
            {
                string_buf<16> srn("srn%d", i);
                InitSirenLight(basename, srn, this->GetGeomId(srn));
            }
        }
    }

    void vehCarModel::InitHeadlights(const char* basename)
    {
        Matrix34 outMatrix = Matrix34::I;

        // load headlights
        int headlightCount = this->GetGeomCount("headlight");

        if (this->headlights)
        {
            delete[] this->headlights;
            this->headlights = nullptr;
        }
        if (this->headlightPositions)
        {
            delete[] this->headlightPositions;
            this->headlightPositions = nullptr;
		}
        if (this->headlightElectrics)
        {
            delete[] this->headlightElectrics;
            this->headlightElectrics = nullptr;
		}
        if (headlightCount > 0)
        {
            this->headlights = new ltLight[headlightCount];
            this->headlightPositions = new Vector3[headlightCount];
			this->headlightElectrics = new bool[headlightCount];

            for (int i = 0; i < headlightCount; i++)
            {
				string_buf<16> headlight("headlight%d", i);
				auto headlightGeom = this->GetGeom(3, this->GetGeomId(headlight));
                if (headlightGeom == nullptr)
					continue;

                GetPivot(outMatrix, basename, headlight);
                headlights[i].Color = Vector3(1.0f, 1.0f, 1.0f);
                headlights[i].Type = 1;
                headlights[i].SpotExponent = 3.0f;
                this->GetSurfaceColor(headlightGeom, headlights[i].Color);
                this->headlightPositions[i] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
            }
        }
    }

    AGE_API void vehCarModel::BreakElectrics(const Vector3& localImpactPos)     
    {
        float radius = 0.25f;
		int headlightCount = this->GetGeomCount("headlight");

        for (int i = 0; i < headlightCount; i++)
        {
            if ((this->headlightPositions[i] - localImpactPos).Mag2() < radius)
                this->headlightElectrics[i] = false;
        }

        auto siren = this->car->GetSiren();
        if (siren != nullptr)
        {
            for (int i = 0; i < siren->GetLightCount(); i++)
            {
                if ((siren->GetLightPosition(i) - localImpactPos).Mag2() < radius)
                    siren->SetLightElectric(i, false);
            }
        }
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
        if (lightDamage)
            lightDamage->Reset(false);
        
        this->hasEjectedOneshot = false;
        this->wheelBrokenStatus = 0xFFFFFFFF;

		int headlightCount = this->GetGeomCount("headlight");
        for (int i = 0; i < headlightCount; i++)
        {
			this->headlightElectrics[i] = true;
        }

        auto siren = this->car->GetSiren();
        if (siren != nullptr)
        {
            for (int i = 0; i < siren->GetLightCount(); i++)
            {
                siren->SetLightElectric(i, true);
            }
        }
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
        if (this->headlights == nullptr || this->GetCustomGlowState())
            return;

        int headlightCount = this->GetGeomCount("headlight");
        float rotationAmount = vehCarModel::HeadlightFlashingSpeed;

        ltLight::DrawGlowBegin();
        for (int i = 0; i < headlightCount; i++)
        {
            if (rotate)
            {
                this->headlights[i].Direction.RotateY(datTimeManager::Seconds * rotationAmount);
                rotationAmount *= -1.f;
            }
            else
            {
                auto carMatrix = &this->GetWorldMatrix();
                this->headlights[i].Direction = Vector3(-carMatrix->m20, -carMatrix->m21, -carMatrix->m22);
            }

            bool isHeadlightBroken = !(headlightElectrics[i]);
            if (isHeadlightBroken)
                continue;

            auto light = &this->headlights[i];
            auto lightPos = this->headlightPositions[i];
            auto carMatrix = &this->GetWorldMatrix();

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

    //new init
    void vehCarModel::Init(vehCar* car, const char* basename, int variant)
    {
        this->car = car;
        this->carSim = car->GetCarSim();
        this->variant = (variant > 255) ? 0 : variant;
        bool hasGeometry = false;

        if (lvlInstance::BeginGeomWithGroup(basename, "body", "player", 0xC)) 
        {
            lvlInstance::AddGeoms(basename, "player");
            lvlInstance::EndGeom();
            hasGeometry = true;
        }

        //clamp variant value
        this->variant = this->variant % this->GetVariantCount();

        //pre-load our variant
        lvlInstance::PreLoadShader(this->variant);

        if (!vehCarModel::MM1StyleDamage)
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
                    if (!texelDamage->Init(bodyEntry->GetHighLOD(), bodyEntry->pShaders[this->GetVariant()], bodyEntry->numShaders))
                    {
                        delete texelDamage;
                        texelDamage = nullptr;
                        Warningf("Vehicle '%s': No damage textures found, disabling texel damage.", basename);
                    }
                }
            }
        }

        if (vehCarModel::MM1StyleDamage && !vehCarModel::Enable3DDamage)
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
                    mm1Damage->Init(bodyEntry->GetHighLOD(), bodyEntry->pShaders[this->GetVariant()], bodyEntry->numShaders);
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
                auto bodyEntry = this->GetGeomBase();
                modStatic* bodyDamageEntry = this->GetGeomEntry(this->GetGeomId("bodydamage"));
                if (bodyEntry->GetHighLOD() != nullptr)
                {
                    this->damage3D = new fxDamage3D();
                    damage3D->Init(bodyEntry->GetHighLOD(), bodyDamageEntry != nullptr ? bodyDamageEntry : bodyEntry->GetHighLOD(), vehCarModel::MM1StyleDamage);
                    damage3D->SetShaders(bodyEntry->pShaders[this->GetVariant()], bodyEntry->numShaders);
                }
            }
        }

        if (vehCarModel::EnableLightDamage)
        {
            //init lightDamage
            if (this->lightDamage != nullptr)
            {
                // delete old lightDamage
                delete this->lightDamage;
                this->lightDamage = nullptr;
            }

            if (this->GetGeomIndex() != 0)
            {
                this->lightDamage = new mmLightDamage();
                modStatic* hlight = this->GetGeomEntry(this->GetGeomId("hlight")); if (hlight != nullptr) lightDamage->Init(hlight, mmLightDamage::HLIGHT);
                modStatic* tlight = this->GetGeomEntry(this->GetGeomId("tlight")); if (tlight != nullptr) lightDamage->Init(tlight, mmLightDamage::TLIGHT);
                modStatic* rlight = this->GetGeomEntry(this->GetGeomId("rlight")); if (rlight != nullptr) lightDamage->Init(rlight, mmLightDamage::RLIGHT);
                modStatic* slight0 = this->GetGeomEntry(this->GetGeomId("slight0")); if (slight0 != nullptr) lightDamage->Init(slight0, mmLightDamage::SLIGHT0);
                modStatic* slight1 = this->GetGeomEntry(this->GetGeomId("slight1")); if (slight1 != nullptr) lightDamage->Init(slight1, mmLightDamage::SLIGHT1);
                modStatic* blight = this->GetGeomEntry(this->GetGeomId("blight")); if (blight != nullptr) lightDamage->Init(blight, mmLightDamage::BLIGHT);
                modStatic* siren0 = this->GetGeomEntry(this->GetGeomId("siren0")); if (siren0 != nullptr) lightDamage->Init(siren0, mmLightDamage::SIREN0);
                modStatic* siren1 = this->GetGeomEntry(this->GetGeomId("siren1")); if (siren1 != nullptr) lightDamage->Init(siren1, mmLightDamage::SIREN1);
                modStatic* tslight0 = this->GetGeomEntry(this->GetGeomId("tslight0")); if (tslight0 != nullptr) lightDamage->Init(tslight0, mmLightDamage::TSLIGHT0);
                modStatic* tslight1 = this->GetGeomEntry(this->GetGeomId("tslight1")); if (tslight1 != nullptr) lightDamage->Init(tslight1, mmLightDamage::TSLIGHT1);
            }
        }

        //optimize this instance
        if (hasGeometry)
            lvlInstance::Optimize(this->variant);

        //init siren lights and headlights
        InitSirenLights(basename);
        InitHeadlights(basename);

        //load FNDR offsets
        if (this->GetGeom(3, this->GetGeomId("fndr0")) != nullptr)
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
        if (this->GetGeom(3, this->GetGeomId("fndr2")) != nullptr)
        {
            Matrix34 outMatrix;
            auto carsim = this->carSim;

            GetPivot(outMatrix, basename, "fndr2");
            this->fndr2offset = outMatrix.GetRow(3) - carsim->GetWheel(2)->GetCenter();

            GetPivot(outMatrix, basename, "fndr3");
            this->fndr3offset = outMatrix.GetRow(3) - carsim->GetWheel(3)->GetCenter();
        }

        //extra FNDR4/5 offsets
        if (this->GetGeom(3, this->GetGeomId("fndr4")) != nullptr)
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

        InitBreakable(this->genBreakableMgr, basename, "break0", 0);
        InitBreakable(this->genBreakableMgr, basename, "break1", 0);
        InitBreakable(this->genBreakableMgr, basename, "break2", 0);
        InitBreakable(this->genBreakableMgr, basename, "break3", 0);
        InitBreakable(this->genBreakableMgr, basename, "break01", 0);
        InitBreakable(this->genBreakableMgr, basename, "break12", 0);
        InitBreakable(this->genBreakableMgr, basename, "break23", 0);
        InitBreakable(this->genBreakableMgr, basename, "break03", 0);
        InitBreakable(this->genBreakableMgr, basename, "lightbar0", 1);
        InitBreakable(this->genBreakableMgr, basename, "lightbar1", 2);
            
        string_buf<16> buffer("variant%d", this->variant);
        InitBreakable(this->genBreakableMgr, basename, buffer, 0);

        //create wheel breakables
        this->wheelBreakableMgr = new vehBreakableMgr();
        this->wheelBreakableMgr->Init(this->carSim->GetWorldMatrix());
        this->wheelBreakableMgr->SetVariant(this->variant);
            
        InitBreakable(this->wheelBreakableMgr, basename, "whl0", 1 << 0);
        InitBreakable(this->wheelBreakableMgr, basename, "hub0", 1 << 1);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr0", 1 << 2);
        InitBreakable(this->wheelBreakableMgr, basename, "whl1", 1 << 3);
        InitBreakable(this->wheelBreakableMgr, basename, "hub1", 1 << 4);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr1", 1 << 5);
        InitBreakable(this->wheelBreakableMgr, basename, "whl2", 1 << 6);
        InitBreakable(this->wheelBreakableMgr, basename, "hub2", 1 << 7);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr2", 1 << 8);
        InitBreakable(this->wheelBreakableMgr, basename, "whl3", 1 << 9);
        InitBreakable(this->wheelBreakableMgr, basename, "hub3", 1 << 10);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr3", 1 << 11);
        InitBreakable(this->wheelBreakableMgr, basename, "whl4", 1 << 12);
        InitBreakable(this->wheelBreakableMgr, basename, "hub4", 1 << 13);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr4", 1 << 14);
        InitBreakable(this->wheelBreakableMgr, basename, "whl5", 1 << 15);
        InitBreakable(this->wheelBreakableMgr, basename, "hub5", 1 << 16);
        InitBreakable(this->wheelBreakableMgr, basename, "fndr5", 1 << 17);
        InitBreakable(this->wheelBreakableMgr, basename, "engine", 1 << 18);

        //load trailer hitch offset
        auto hitchGeom = this->GetGeom(3, this->GetGeomId("trailer_hitch"));
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
    AGE_API const Matrix34 & vehCarModel::GetMatrix(Matrix34 &a1)
                                                        { return hook::Thunk<0x4CEF90>::Call<const Matrix34 &>(this, &a1); }
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
        if (this->GetVariant() == 4 && !strcmp(this->GetName(), "vppanozgt_body"))
            return; // Skip drawing the Secret Shadow paintjob for Panoz GTR-1 here

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

        if (Enable3DDamage && MM1StyleDamage)
        {
            //use 3d damage
            if (lod >= 2 && this->damage3D != nullptr)
            {
                shaders = this->damage3D->GetCleanShaders();
            }
        }

        //setup renderer
        gfxRenderState::SetWorldMatrix(this->GetWorldMatrix());

        //draw the body
        modStatic* bodyGeom = this->GetGeom(lod, 0);
        if (Enable3DDamage)
        {
            if (damage3D != nullptr)
            {
                bodyGeom = lod >= 2 ? damage3D->GetDeformModel() : this->GetGeom(lod, 0);
                if (bodyGeom != nullptr)
                    bodyGeom->DrawNoGlass(shaders);

                if (MM1StyleDamage)
                {
                    modStatic* bodyDamage = lod >= 2 ? damage3D->GetDeformDamageModel() : this->GetGeom(lod, 0);
                    if (bodyDamage != nullptr)
                        bodyDamage->DrawNoGlass(lod >= 2 ? damage3D->GetDamageShaders() : shaders);
                }
            }
        }
        else if (MM1StyleDamage)
        {
            if (mm1Damage != nullptr)
            {
                bodyGeom = lod >= 2 ? mm1Damage->GetCleanModel() : this->GetGeom(lod, 0);
                if (bodyGeom != nullptr)
                    bodyGeom->DrawNoGlass(shaders);

                modStatic* bodyDamage = lod >= 2 ? mm1Damage->GetDamageModel() : this->GetGeom(lod, 0);
                if (bodyDamage != nullptr)
                    bodyDamage->DrawNoGlass(lod >= 2 ? mm1Damage->GetDamageShaders() : shaders);
            }
        }
        else
        {
            if (bodyGeom != nullptr)
                bodyGeom->DrawNoGlass(shaders);
        }

        //draw breakables
        this->genBreakableMgr->Draw(this->GetWorldMatrix(), shaders, lod);

        //draw decal
        auto decalGeom = this->GetGeom(lod, this->GetGeomId("decal"));
        if (decalGeom != nullptr)
        {
            auto oldAlphaRef = gfxRenderState::SetAlphaRef(0);
            DrawPart(decalGeom, this->GetWorldMatrix(), shaders);
            gfxRenderState::SetAlphaRef(oldAlphaRef);
        }

        //draw pop-up lights
        if (lod >= 1)
        {
            if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                //plighton
                DrawPart(lod, this->GetGeomId("plighton"), this->GetWorldMatrix(), shaders);
            else
                //plightoff
                DrawPart(lod, this->GetGeomId("plightoff"), this->GetWorldMatrix(), shaders);
        }

        //get wheels
        auto carSim = this->carSim;
        vehWheel* wheels[6] = { carSim->GetWheel(0), carSim->GetWheel(1), carSim->GetWheel(2), carSim->GetWheel(3), carSim->GetWheel(2), carSim->GetWheel(3) };

        //draw fenders
        if (lod == 3)
        {
            for (int i = 0; i < 6; i++)
            {
                string_buf<16> fndr("fndr%d", i);
                int fndrId = this->GetGeomId(fndr);
                int fndrStatusFlag = 1 << ((i * 3) + 2);
                auto fndrGeom = this->GetGeom(3, fndrId);
                if (fndrGeom != nullptr && (wheelBrokenStatus & fndrStatusFlag) != 0)
                {
                    DrawPart(lod, fndrId, this->GetFenderMatrix(i), shaders);
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
                    string_buf<16> shub("shub%d", i);
                    string_buf<16> hub("hub%d", i);
                    int shubId = this->GetGeomId(shub);
                    int hubId = this->GetGeomId(hub);
                    auto shubGeom = this->GetGeom(lod, shubId);
                    if (fabs(wheel->GetRotationRate()) >= BlurSpeed && shubGeom != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, shubId, this->GetWheelMatrix(i), shaders);
                    }
                    else
                    {
                        DrawPart(lod, hubId, this->GetWheelMatrix(i), shaders);
                    }
                }

                //whl
                int whlStatusFlag = 1 << (i * 3);
                if ((this->wheelBrokenStatus & whlStatusFlag) != 0)
                {
                    string_buf<16> swhl("swhl%d", i);
                    string_buf<16> whl("whl%d", i);
                    int swhlId = this->GetGeomId(swhl);
                    int whlId = this->GetGeomId(whl);
                    auto swhlGeom = this->GetGeom(lod, swhlId);
                    if (fabs(wheel->GetRotationRate()) >= BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, swhlId, this->GetWheelMatrix(i), shaders);
                    }
                    else
                    {
                        DrawPart(lod, whlId, this->GetWheelMatrix(i), shaders);
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
                auto shock = &this->carSim->ShockSuspensions[i];
                Matrix34 shockMatrix = vehCarModel::EnableWheelWobble ? shock->GetSuspensionPivot() : shock->GetSuspensionMatrix();
                string_buf<16> buffer("shock%d", i);
                DrawPart(lod, this->GetGeomId(buffer), GetPartWobbleMatrix(shockMatrix, shock->GetWheel()), shaders);
            }
            for (int i = 0; i < 4; i++)
            {
                auto arm = &this->carSim->ArmSuspensions[i];
                Matrix34 armMatrix = vehCarModel::EnableWheelWobble ? arm->GetSuspensionPivot() : arm->GetSuspensionMatrix();
                string_buf<16> buffer("arm%d", i);
                DrawPart(lod, this->GetGeomId(buffer), GetPartWobbleMatrix(armMatrix, arm->GetWheel()), shaders);
            }
            for (int i = 0; i < 2; i++)
            {
                auto shaft = &this->carSim->ShaftSuspensions[i];
                Matrix34 shaftMatrix = vehCarModel::EnableWheelWobble ? shaft->GetSuspensionPivot() : shaft->GetSuspensionMatrix();
                string_buf<16> buffer("shaft%d", i + 2);
                DrawPart(lod, this->GetGeomId(buffer), GetPartWobbleMatrix(shaftMatrix, shaft->GetWheel()), shaders);
            }

            auto axleFront = &this->carSim->AxleFront;
            Matrix34 axleFrontMatrix = vehCarModel::EnableWheelWobble ? axleFront->GetAxlePivot() : axleFront->GetAxleMatrix();
            DrawPart(lod, this->GetGeomId("axle0"), GetPartWobbleMatrix(axleFrontMatrix, axleFront->GetRightWheel()), shaders);

            auto axleRear = &this->carSim->AxleRear;
            Matrix34 axleRearMatrix = vehCarModel::EnableWheelWobble ? axleRear->GetAxlePivot() : axleRear->GetAxleMatrix();
            DrawPart(lod, this->GetGeomId("axle1"), GetPartWobbleMatrix(axleRearMatrix, axleRear->GetLeftWheel()), shaders);
                
            //engine
            if ((this->wheelBrokenStatus & 0x40000) != 0)
            {
                auto engineMatrixPtr = this->carSim->GetEngine()->GetVisualMatrixPtr();
                if(engineMatrixPtr != nullptr)
                    DrawPart(lod, this->GetGeomId("engine"), *engineMatrixPtr, shaders);
            }
        }
    }

    AGE_API void vehCarModel::DrawShadow()
    {
        if (vehCarModel::Enable3DShadows <= 1
            || MMSTATE->TimeOfDay == 3
            || MMSTATE->WeatherType != 0
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
        modStatic* model = damage3D != nullptr ? damage3D->GetDeformModel() : this->GetGeomBase()->GetHighestLOD();

        if (model != nullptr)
        {
            Matrix34 shadowMatrix = Matrix34();
            Matrix34 carMatrix = *this->carSim->GetWorldMatrix();

            if (lvlInstance::ComputeShadowProjectionMatrix(shadowMatrix, this->GetRoomId(), timeWeather->KeyPitch, timeWeather->KeyHeading, carMatrix, this))
            {
                gfxRenderState::SetWorldMatrix(shadowMatrix);
                float intensity = ComputeShadowIntensity(timeWeather->KeyColor);
                model->DrawShadowed(shaders, intensity);

                if (MM1StyleDamage && damage3D != nullptr) damage3D->GetDeformDamageModel()->DrawShadowed(shaders, intensity);

                //draw breakables
                this->genBreakableMgr->DrawShadowed(shadowMatrix, shaders, intensity);

                if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                    //plighton
                    DrawPartShadowed(3, this->GetGeomId("plighton"), shadowMatrix, shaders, intensity);
                else
                    //plightoff
                    DrawPartShadowed(3, this->GetGeomId("plightoff"), shadowMatrix, shaders, intensity);

                //get wheels
                auto carSim = this->carSim;
                vehWheel* wheels[6] = { carSim->GetWheel(0), carSim->GetWheel(1), carSim->GetWheel(2), carSim->GetWheel(3), carSim->GetWheel(2), carSim->GetWheel(3) };

                //draw wheels, hubs and fenders
                for (int i = 0; i < 6; i++)
                {
                    auto wheel = wheels[i];
                    int whlStatusFlag = 1 << (i * 3);
                    int hubStatusFlag = 1 << ((i * 3) + 1);
                    int fndrStatusFlag = 1 << ((i * 3) + 2);

                    //fndr
                    string_buf<16> fndr("fndr%d", i);
                    int fndrId = this->GetGeomId(fndr);
                    auto fndrGeom = this->GetGeom(3, fndrId);
                    if (fndrGeom != nullptr && (wheelBrokenStatus & fndrStatusFlag) != 0)
                    {
                        DrawPartShadowed(3, fndrId, this->GetFenderShadowMatrix(i, shadowMatrix), shaders, intensity);
                    }

                    //hub
                    string_buf<16> hub("hub%d", i);
                    int hubId = this->GetGeomId(hub);
                    auto hubGeom = this->GetGeom(3, hubId);
                    if (hubGeom != nullptr && (this->wheelBrokenStatus & hubStatusFlag) != 0)
                    {
                        string_buf<16> shub("shub%d", i);
                        int shubId = this->GetGeomId(shub);
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
                    string_buf<16> whl("whl%d", i);
                    int whlId = this->GetGeomId(whl);
                    auto whlGeom = this->GetGeom(3, whlId);
                    if (whlGeom != nullptr && (this->wheelBrokenStatus & whlStatusFlag) != 0)
                    {
                        string_buf<16> swhl("swhl%d", i);
                        int swhlId = this->GetGeomId(swhl);
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
                    string_buf<16> arm("arm%d", i);
                    int armGeomId = this->GetGeomId(arm);
                    auto armGeom = this->GetGeom(3, armGeomId);
                    if (armGeom != nullptr)
                    {
                        Matrix34 armShadowMatrix = Matrix34();
                        armShadowMatrix.Identity();
                        armShadowMatrix.SetRow(3, this->carSim->ArmSuspensions[i].GetSuspensionPivot().GetRow(3));

                        // Rotate arm 90 degrees if id is even, if it's odd then rotate -90 degrees
                        // This case is for Panoz Roadster arm parts only
                        if (strstr(this->GetName(), "vppanoz_arm"))
                            armShadowMatrix.RotateY((i % 2 == 0) ? 1.570796f : -1.570796f);

                        armShadowMatrix.ScaleFull(1.0f - this->carSim->GetWheel(i)->GetBumpDisplacement(), 1.0f, 1.0f);
                        armShadowMatrix.Dot(shadowMatrix);
                        DrawPartShadowed(3, armGeomId, armShadowMatrix, shaders, intensity);
                    }
                }

                //engine
                int engineGeomId = this->GetGeomId("engine");
                auto engineGeom = this->GetGeom(3, engineGeomId);
                if (engineGeom != nullptr && (this->wheelBrokenStatus & 0x40000) != 0)
                {
                    auto engineMatrixPtr = this->carSim->GetEngine()->GetVisualMatrixPtr();
                    if (engineMatrixPtr != nullptr)
                    {
                        Matrix34 engineShadowMatrix = Matrix34();
                        if (lvlInstance::ComputeShadowProjectionMatrix(engineShadowMatrix, this->GetRoomId(), timeWeather->KeyPitch, timeWeather->KeyHeading, *engineMatrixPtr, this))
                        {
                            DrawPartShadowed(3, engineGeomId, engineShadowMatrix, shaders, intensity);
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
        auto siren = car->GetSiren();
        auto curDamage = car->GetCarDamage()->GetCurDamage();
        auto maxDamage = car->GetCarDamage()->GetMaxDamage();
        int gear = carSim->GetTransmission()->GetGear();
        if (curDamage >= maxDamage && vehCarModel::MWStyleTotaledCar)
            return;

        //setup renderer
        gfxRenderState::SetWorldMatrix(this->GetWorldMatrix());

        //draw signals
        modStatic* slight0 = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::SLIGHT0) : this->GetGeomEntry(this->GetGeomId("slight0"));
        modStatic* slight1 = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::SLIGHT1) : this->GetGeomEntry(this->GetGeomId("slight1"));

        //draw brake signals
        modStatic* tslight0 = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::TSLIGHT0) : this->GetGeomEntry(this->GetGeomId("tslight0"));
        modStatic* tslight1 = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::TSLIGHT1) : this->GetGeomEntry(this->GetGeomId("tslight1"));

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
        if (car->IsPlayer())
        {
            if (!LeftSignalLightState && !HazardLightsState)
            {
                if (tslight0 != nullptr)
                {
                    //draw brake copy
                    if (carSim->GetBrake() > 0.1)
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
                    if (carSim->GetBrake() > 0.1)
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
                if (carSim->GetBrake() > 0.1)
                    tslight0->Draw(shaders);
                //draw headlight copy
                if (vehCar::sm_DrawHeadlights)
                    tslight0->Draw(shaders);
            }
            if (tslight1 != nullptr)
            {
                //draw brake copy
                if (carSim->GetBrake() > 0.1)
                    tslight1->Draw(shaders);
                //draw headlight copy
                if (vehCar::sm_DrawHeadlights)
                    tslight1->Draw(shaders);
            }
        }

        //draw tlight
        modStatic* tlight = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::TLIGHT) : this->GetGeomEntry(this->GetGeomId("tlight"));
        if (tlight != nullptr)
        {
            //draw brake copy
            if (carSim->GetBrake() > 0.1)
                tlight->Draw(shaders);
            //draw headlight copy
            if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                tlight->Draw(shaders);
        }

        //draw blight
        modStatic* blight = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::BLIGHT) : this->GetGeomEntry(this->GetGeomId("blight"));
        if (blight != nullptr)
        {
            //draw brake copy
            if (carSim->GetBrake() > 0.1)
                blight->Draw(shaders);
        }

        //draw rlight
        modStatic* rlight = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::RLIGHT) : this->GetGeomEntry(this->GetGeomId("rlight"));
        if (MM1StyleTransmission)
        {
            auto throttle = carSim->GetEngine()->GetThrottleInput();
            auto speedMPH = carSim->GetSpeedMPH();
            auto transmission = carSim->GetTransmission();

            if (rlight != nullptr && gear == 0)
            {
                if (transmission->IsAuto())
                {
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

        //Draw siren and headlights
        modStatic* hlight = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::HLIGHT) : this->GetGeomEntry(this->GetGeomId("hlight"));
        modStatic* siren0 = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::SIREN0) : this->GetGeomEntry(this->GetGeomId("siren0"));
        modStatic* siren1 = lightDamage != nullptr ? lightDamage->GetLightModel(mmLightDamage::SIREN1) : this->GetGeomEntry(this->GetGeomId("siren1"));

        if (vehCarModel::HeadlightType < 3)
        {
            if (vehCarModel::HeadlightType == 0 || vehCarModel::HeadlightType == 2)
            {
                //MM2 headlights
                if (siren != nullptr && siren->IsActive() && !siren->GetCustomGlowState() && vehCarModel::EnableHeadlightFlashing)
                {
                    this->DrawHeadlights(true);
                }
                else if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                {
                    this->DrawHeadlights(false);
                }
            }
            if (vehCarModel::HeadlightType == 1 || vehCarModel::HeadlightType == 2)
            {
                //MM1 headlights
                gfxRenderState::SetWorldMatrix(this->GetWorldMatrix());

                if (hlight != nullptr)
                {
                    if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                        hlight->Draw(shaders);
                }
            }
        }

        if (vehCarModel::SirenType < 3)
        {
            if (vehCarModel::SirenType == 0 || vehCarModel::SirenType == 2)
            {
                //MM2 siren
                if (siren != nullptr && siren->IsActive())
                    siren->Draw(this->GetWorldMatrix());
            }
            if (vehCarModel::SirenType == 1 || vehCarModel::SirenType == 2)
            {
                //MM1 siren
                gfxRenderState::SetWorldMatrix(this->GetWorldMatrix());

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

        if (Enable3DDamage && MM1StyleDamage)
        {
            //use 3d damage
            if (this->damage3D != nullptr)
            {
                shaders = this->damage3D->GetCleanShaders();
            }
        }

        //setup renderer
        gfxRenderState::SetWorldMatrix(this->GetWorldMatrix());

        //draw the body
        modStatic* bodyGeom = this->GetGeom(3, 0);
        if (Enable3DDamage)
        {
            if (damage3D != nullptr)
            {
                bodyGeom = damage3D->GetDeformModel();

                if (MM1StyleDamage && DamageReflections)
                {
                    modStatic* bodyDamage = damage3D->GetDeformDamageModel();
                    if (bodyDamage != nullptr)
                        bodyDamage->DrawReflected(shaders, this->GetRoomId(), this->GetWorldMatrix(), intensity);
                }
            }
        }
        else if (MM1StyleDamage)
        {
            if (mm1Damage != nullptr)
            {
                bodyGeom = mm1Damage->GetCleanModel();

                if (DamageReflections)
                {
                    modStatic* bodyDamage = mm1Damage->GetDamageModel();
                    if (bodyDamage != nullptr)
                        bodyDamage->DrawReflected(shaders, this->GetRoomId(), this->GetWorldMatrix(), intensity);
                }
            }
        }

        if (bodyGeom != nullptr)
            bodyGeom->DrawReflected(shaders, this->GetRoomId(), this->GetWorldMatrix(), intensity);

        //draw breakables
        this->genBreakableMgr->DrawReflected(this->GetWorldMatrix(), shaders, 3, this->GetRoomId());

        //draw pop-up lights
        if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
            //plighton
            DrawPartReflected(3, this->GetGeomId("plighton"), this->GetWorldMatrix(), shaders, intensity, vehCarModel::PartReflections);
        else
            //plightoff
            DrawPartReflected(3, this->GetGeomId("plightoff"), this->GetWorldMatrix(), shaders, intensity, vehCarModel::PartReflections);

        //get wheels
        auto carSim = this->carSim;
        vehWheel* wheels[6] = { carSim->GetWheel(0), carSim->GetWheel(1), carSim->GetWheel(2), carSim->GetWheel(3), carSim->GetWheel(2), carSim->GetWheel(3) };

        //draw fenders
        for (int i = 0; i < 6; i++)
        {
            string_buf<16> fndr("fndr%d", i);
            int fndrId = this->GetGeomId(fndr);
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
                string_buf<16> shub("shub%d", i);
                string_buf<16> hub("hub%d", i);
                int shubId = this->GetGeomId(shub);
                int hubId = this->GetGeomId(hub);
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
                string_buf<16> swhl("swhl%d", i);
                string_buf<16> whl("whl%d", i);
                int swhlId = this->GetGeomId(swhl);
                int whlId = this->GetGeomId(whl);
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
            auto shock = &this->carSim->ShockSuspensions[i];
            Matrix34 shockMatrix = vehCarModel::EnableWheelWobble ? shock->GetSuspensionPivot() : shock->GetSuspensionMatrix();
            string_buf<16> buffer("shock%d", i);
            DrawPartReflected(3, this->GetGeomId(buffer), GetPartWobbleMatrix(shockMatrix, shock->GetWheel()), shaders, intensity, vehCarModel::PartReflections);
        }
        for (int i = 0; i < 4; i++)
        {
            auto arm = &this->carSim->ArmSuspensions[i];
            Matrix34 armMatrix = vehCarModel::EnableWheelWobble ? arm->GetSuspensionPivot() : arm->GetSuspensionMatrix();
            string_buf<16> buffer("arm%d", i);
            DrawPartReflected(3, this->GetGeomId(buffer), GetPartWobbleMatrix(armMatrix, arm->GetWheel()), shaders, intensity, vehCarModel::PartReflections);
        }
        for (int i = 0; i < 2; i++)
        {
            auto shaft = &this->carSim->ShaftSuspensions[i];
            Matrix34 shaftMatrix = vehCarModel::EnableWheelWobble ? shaft->GetSuspensionPivot() : shaft->GetSuspensionMatrix();
            string_buf<16> buffer("shaft%d", i + 2);
            DrawPartReflected(3, this->GetGeomId(buffer), GetPartWobbleMatrix(shaftMatrix, shaft->GetWheel()), shaders, intensity, vehCarModel::PartReflections);
        }

        auto axleFront = &this->carSim->AxleFront;
        Matrix34 axleFrontMatrix = vehCarModel::EnableWheelWobble ? axleFront->GetAxlePivot() : axleFront->GetAxleMatrix();
        DrawPartReflected(3, this->GetGeomId("axle0"), GetPartWobbleMatrix(axleFrontMatrix, axleFront->GetRightWheel()), shaders, intensity, vehCarModel::PartReflections);

        auto axleRear = &this->carSim->AxleRear;
        Matrix34 axleRearMatrix = vehCarModel::EnableWheelWobble ? axleRear->GetAxlePivot() : axleRear->GetAxleMatrix();
        DrawPartReflected(3, this->GetGeomId("axle1"), GetPartWobbleMatrix(axleRearMatrix, axleRear->GetLeftWheel()), shaders, intensity, vehCarModel::PartReflections);

        //engine
        if ((this->wheelBrokenStatus & 0x40000) != 0)
        {
            auto engineMatrixPtr = this->carSim->GetEngine()->GetVisualMatrixPtr();
            if (engineMatrixPtr != nullptr)
                DrawPartReflected(3, this->GetGeomId("engine"), *engineMatrixPtr, shaders, intensity, vehCarModel::PartReflections);
        }
    }

    AGE_API void vehCarModel::DrawReflectedParts(int lod)
    {
        if (lod < 0 || lod > 3)
            return;
        if (this->GetGeomIndex() == 0)
            return;

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

        if (Enable3DDamage && MM1StyleDamage)
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

		bool secretShadow = this->GetVariant() == 4 && !strcmp(this->GetName(), "vppanozgt_body");
        if (secretShadow)
        {
            gfxRenderState::SetAlphaRef(0);
            gfxRenderState::SetZWriteEnabled(false);
        }

        //setup renderer
        gfxRenderState::SetWorldMatrix(this->GetWorldMatrix());

        //draw the body
        modStatic* bodyGeom = this->GetGeom(lod, 0);
        if (Enable3DDamage)
        {
            if (damage3D != nullptr)
            {
                bodyGeom = lod >= 2 ? damage3D->GetDeformModel() : this->GetGeom(lod, 0);
                if (bodyGeom != nullptr)
                    secretShadow ? bodyGeom->DrawAlpha(shaders) : bodyGeom->DrawGlass(shaders);

                if (MM1StyleDamage)
                {
                    modStatic* bodyDamage = lod >= 2 ? damage3D->GetDeformDamageModel() : this->GetGeom(lod, 0);
                    if (bodyDamage != nullptr)
                    {
						auto currentShaders = lod >= 2 ? damage3D->GetDamageShaders() : shaders;
                        secretShadow ? bodyDamage->DrawAlpha(currentShaders) : bodyDamage->DrawGlass(currentShaders);
                    }
                }
            }
        }
        else if (MM1StyleDamage)
        {
            if (mm1Damage != nullptr)
            {
                bodyGeom = lod >= 2 ? mm1Damage->GetCleanModel() : this->GetGeom(lod, 0);
                if (bodyGeom != nullptr)
                    secretShadow ? bodyGeom->DrawAlpha(shaders) : bodyGeom->DrawGlass(shaders);

                modStatic* bodyDamage = lod >= 2 ? mm1Damage->GetDamageModel() : this->GetGeom(lod, 0);
                if (bodyDamage != nullptr)
                {
					auto currentShaders = lod >= 2 ? mm1Damage->GetDamageShaders() : shaders;
                    secretShadow ? bodyDamage->DrawAlpha(currentShaders) : bodyDamage->DrawGlass(currentShaders);
                }
            }
        }
        else
        {
            if (bodyGeom != nullptr)
                secretShadow ? bodyGeom->DrawAlpha(shaders) : bodyGeom->DrawGlass(shaders);
        }

        if (!secretShadow)
            return;

        //draw breakables
        this->genBreakableMgr->Draw(this->GetWorldMatrix(), shaders, lod);

        //draw decal
        auto decalGeom = this->GetGeom(lod, this->GetGeomId("decal"));
        if (decalGeom != nullptr)
        {
            auto oldAlphaRef2 = gfxRenderState::SetAlphaRef(0);
            DrawPart(decalGeom, this->GetWorldMatrix(), shaders);
            gfxRenderState::SetAlphaRef(oldAlphaRef2);
        }

        //draw pop-up lights
        if (lod >= 1)
        {
            if (car->IsPlayer() && vehCarModel::ShowHeadlights || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                //plighton
                DrawPart(lod, this->GetGeomId("plighton"), this->GetWorldMatrix(), shaders);
            else
                //plightoff
                DrawPart(lod, this->GetGeomId("plightoff"), this->GetWorldMatrix(), shaders);
        }

        //get wheels
        auto carSim = this->carSim;
        vehWheel* wheels[6] = { carSim->GetWheel(0), carSim->GetWheel(1), carSim->GetWheel(2), carSim->GetWheel(3), carSim->GetWheel(2), carSim->GetWheel(3) };

        //draw fenders
        if (lod == 3)
        {
            for (int i = 0; i < 6; i++)
            {
                string_buf<16> fndr("fndr%d", i);
                int fndrId = this->GetGeomId(fndr);
                int fndrStatusFlag = 1 << ((i * 3) + 2);
                auto fndrGeom = this->GetGeom(3, fndrId);
                if (fndrGeom != nullptr && (wheelBrokenStatus & fndrStatusFlag) != 0)
                {
                    DrawPart(lod, fndrId, this->GetFenderMatrix(i), shaders);
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
                    string_buf<16> shub("shub%d", i);
                    string_buf<16> hub("hub%d", i);
                    int shubId = this->GetGeomId(shub);
                    int hubId = this->GetGeomId(hub);
                    auto shubGeom = this->GetGeom(lod, shubId);
                    if (fabs(wheel->GetRotationRate()) >= BlurSpeed && shubGeom != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, shubId, this->GetWheelMatrix(i), shaders);
                    }
                    else
                    {
                        DrawPart(lod, hubId, this->GetWheelMatrix(i), shaders);
                    }
                }

                //whl
                int whlStatusFlag = 1 << (i * 3);
                if ((this->wheelBrokenStatus & whlStatusFlag) != 0)
                {
                    string_buf<16> swhl("swhl%d", i);
                    string_buf<16> whl("whl%d", i);
                    int swhlId = this->GetGeomId(swhl);
                    int whlId = this->GetGeomId(whl);
                    auto swhlGeom = this->GetGeom(lod, swhlId);
                    if (fabs(wheel->GetRotationRate()) >= BlurSpeed && swhlGeom != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, swhlId, this->GetWheelMatrix(i), shaders);
                    }
                    else
                    {
                        DrawPart(lod, whlId, this->GetWheelMatrix(i), shaders);
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
                auto shock = &this->carSim->ShockSuspensions[i];
                Matrix34 shockMatrix = vehCarModel::EnableWheelWobble ? shock->GetSuspensionPivot() : shock->GetSuspensionMatrix();
                string_buf<16> buffer("shock%d", i);
                DrawPart(lod, this->GetGeomId(buffer), GetPartWobbleMatrix(shockMatrix, shock->GetWheel()), shaders);
            }
            for (int i = 0; i < 4; i++)
            {
                auto arm = &this->carSim->ArmSuspensions[i];
                Matrix34 armMatrix = vehCarModel::EnableWheelWobble ? arm->GetSuspensionPivot() : arm->GetSuspensionMatrix();
                string_buf<16> buffer("arm%d", i);
                DrawPart(lod, this->GetGeomId(buffer), GetPartWobbleMatrix(armMatrix, arm->GetWheel()), shaders);
            }
            for (int i = 0; i < 2; i++)
            {
                auto shaft = &this->carSim->ShaftSuspensions[i];
                Matrix34 shaftMatrix = vehCarModel::EnableWheelWobble ? shaft->GetSuspensionPivot() : shaft->GetSuspensionMatrix();
                string_buf<16> buffer("shaft%d", i + 2);
                DrawPart(lod, this->GetGeomId(buffer), GetPartWobbleMatrix(shaftMatrix, shaft->GetWheel()), shaders);
            }

            auto axleFront = &this->carSim->AxleFront;
            Matrix34 axleFrontMatrix = vehCarModel::EnableWheelWobble ? axleFront->GetAxlePivot() : axleFront->GetAxleMatrix();
            DrawPart(lod, this->GetGeomId("axle0"), GetPartWobbleMatrix(axleFrontMatrix, axleFront->GetRightWheel()), shaders);

            auto axleRear = &this->carSim->AxleRear;
            Matrix34 axleRearMatrix = vehCarModel::EnableWheelWobble ? axleRear->GetAxlePivot() : axleRear->GetAxleMatrix();
            DrawPart(lod, this->GetGeomId("axle1"), GetPartWobbleMatrix(axleRearMatrix, axleRear->GetLeftWheel()), shaders);

            //engine
            if ((this->wheelBrokenStatus & 0x40000) != 0)
            {
                auto engineMatrixPtr = this->carSim->GetEngine()->GetVisualMatrixPtr();
                if (engineMatrixPtr != nullptr)
                    DrawPart(lod, this->GetGeomId("engine"), *engineMatrixPtr, shaders);
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
            .addPropertyReadOnly("WorldMatrix", &GetWorldMatrix)
            .addProperty("Variant", &GetVariant, &SetVariant)
            .addProperty("Visible", &GetVisible, &SetVisible)
            .addProperty("CustomGlowState", &GetCustomGlowState, &SetCustomGlowState)

            .addStaticVariableRef("ShowHeadlights", &vehCarModel::ShowHeadlights)
            .addStaticVariableRef("LeftSignalLightState", &vehCarModel::LeftSignalLightState)
            .addStaticVariableRef("RightSignalLightState", &vehCarModel::RightSignalLightState)
            .addStaticVariableRef("HazardLightsState", &vehCarModel::HazardLightsState)

            //lua functions
            .addFunction("GetSurfaceColor", &getSurfaceColorLua)

            //functions
            .addFunction("Reset", &Reset)
            .addFunction("BreakElectrics", &BreakElectrics)
            .addFunction("ClearDamage", &ClearDamage)
            .addFunction("EjectOneshot", &EjectOneshot)

            .addFunction("GetHeadlight", &GetHeadlight)
            .addFunction("GetHeadlightPosition", &GetHeadlightPosition)
            .addFunction("GetHeadlightElectric", &GetHeadlightElectric)


        .endClass();
    }

}