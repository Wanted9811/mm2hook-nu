#pragma once
#include "car.h"

using namespace MM2;

float weatherFriction = vehWheel::GetWeatherFriction();

void vehCar::setDrivable(bool drivable, int mode)
{
	this->SetDrivable(drivable ? TRUE : FALSE, mode);
}
		
AGE_API vehCar::vehCar(BOOL a1)                             { hook::Thunk<0x42BAB0>::Call<void>(this, a1); }
AGE_API vehCar::~vehCar()                                   { hook::Thunk<0x42BCC0>::Call<void>(this); }

declfield(vehCar::sm_DrawHeadlights)(0x627518);

vehCarDamage * vehCar::GetCarDamage(void) const {
	return this->CarDamage;
};

vehCarSim * vehCar::GetCarSim() const {
	return this->CarSim;
}

vehCarModel * vehCar::GetModel() const {
	return this->CarModel;
}

vehSplash * vehCar::GetSplash() const {
	return this->Splash;
}

vehCarAudioContainer * vehCar::GetCarAudioContainerPtr() const {
	return this->CarAudioContainer;
}

vehTrailer * vehCar::GetTrailer() const {
	return this->Trailer;
}

vehSiren * vehCar::GetSiren() const {
	return this->Siren;
}

vehWheelPtx * vehCar::GetWheelPtx() const {
	return this->WheelPtx;
}

vehGyro * vehCar::GetGyro() const {
	return this->Gyro;
}

vehStuck * vehCar::GetStuck() const {
	return this->Stuck;
}

lvlTrackManager* vehCar::GetWheelTrackManager(int num)
{
	switch (num) {
	case 0:
		return &this->WHL0TrackManager;
	case 1:
		return &this->WHL1TrackManager;
	case 2:
		return &this->WHL2TrackManager;
	case 3:
		return &this->WHL3TrackManager;
	}
	return nullptr;
}

AGE_API void vehCar::Reset()                                { hook::Thunk<0x42C330>::Call<void>(this); }

AGE_API void vehCar::ClearDamage()                          { hook::Thunk<0x42C450>::Call<void>(this); }
AGE_API bool vehCar::IsPlayer()                             { return hook::Thunk<0x42C890>::Call<bool>(this); }
AGE_API void vehCar::Init(char const *basename, int variant, int colliderID, bool useFullBound, bool hasTrailer)
															{ hook::Thunk<0x42BE10>::Call<void>(this, basename, variant, colliderID, useFullBound, hasTrailer); }
AGE_API void vehCar::InitAudio(char const *basename, int audioType)      
															{ hook::Thunk<0x42C1F0>::Call<void>(this, basename, audioType); }
AGE_API void vehCar::SetDrivable(BOOL drivable, int mode)   { hook::Thunk<0x42C2C0>::Call<void>(this, drivable, mode); }

AGE_API void vehCar::SetColliderID(int id)
{
	hook::Thunk<0x42C1D0>::Call<void>(this, id);
}

AGE_API void vehCar::DrawTracks()
{
	hook::Thunk<0x42C5E0>::Call<void>(this);
}

/*
	dgPhysEntity virtuals
*/

AGE_API bool vehCar::RequiresTerrainCollision()
												   { return hook::Thunk<0x42CA90>::Call<bool>(this); }
AGE_API lvlInstance* vehCar::GetInst()             { return hook::Thunk<0x42CA80>::Call<lvlInstance *>(this); }
AGE_API phInertialCS* vehCar::GetICS()             { return hook::Thunk<0x42CA70>::Call<phInertialCS *>(this); }
AGE_API void vehCar::PostUpdate()                  { hook::Thunk<0x42C8B0>::Call<void>(this); }
AGE_API void vehCar::Update()                      { hook::Thunk<0x42C690>::Call<void>(this); }
AGE_API void vehCar::PreUpdate()
{
	if ((this->Flags & 2) == 0)
	{
		switch (this->StopMode)
		{
		case 1:
			if (this->GetCarDamage()->GetCurDamage() < this->GetCarDamage()->GetMaxDamage())
			{
				vehWheel::SetWeatherFriction(0.15f);
				if (!this->IsPlayer())
				{
					this->CarSim->GetEngine()->SetThrottleInput(1.0f);
					this->CarSim->SetHandbrake(0.f);
				}
				this->CarSim->GetTransmission()->SetGearChangeTimer(0.f);
				this->CarSim->GetICS()->SetVelocity(Vector3(0.0f, this->CarSim->GetICS()->GetVelocity().Y, 0.0f));
				this->CarSim->GetICS()->SetForce(Vector3(0.0f, this->CarSim->GetICS()->GetForce().Y, 0.0f));
			}
			else
			{
				this->CarSim->SetBrake(1.0f);
			}
			break;
		case 2:
			this->CarSim->SetBrake(1.0f);
			this->CarSim->GetEngine()->SetThrottleInput(0.f);
			this->CarSim->SetSteering(0.f);
			this->CarSim->SetHandbrake(0.f);
			break;
		case 3:
			this->CarSim->SetBrake(1.0f);
			this->CarSim->GetEngine()->SetThrottleInput(0.f);
			this->CarSim->SetSteering(0.f);
			this->CarSim->SetHandbrake(0.f);
			break;
		}
	}
	else
	{
		char* vehName = this->CarDamage->GetName();
		if (!this->CarAudioContainer->IsPolice(vehName))
		{
			vehWheel::SetWeatherFriction(weatherFriction);
		}
	}

	if (this->WheelPtx != nullptr)
	{
		if (this->WheelPtx->isActive())
			lvlLevel::GetSingleton()->SetPtxHeight(*this->WheelPtx->GetParticles());
	}
}

void vehCar::BindLua(LuaState L) {
	LuaBinding(L).beginExtendClass<vehCar, dgPhysEntity>("vehCar")
		.addFactory([]() {
			auto object = new vehCar(TRUE);
			MM2Lua::MarkForCleanupOnShutdown(object);
			return object;
		})

		//properties
		.addPropertyReadOnly("CarDamage", &GetCarDamage)
		.addPropertyReadOnly("CarSim", &GetCarSim)
		.addPropertyReadOnly("Splash", &GetSplash)
		.addPropertyReadOnly("Audio", &GetCarAudioContainerPtr)
		.addPropertyReadOnly("Trailer", &GetTrailer)
		.addPropertyReadOnly("Siren", &GetSiren)
		.addPropertyReadOnly("WheelPtx", &GetWheelPtx)
		.addPropertyReadOnly("Gyro", &GetGyro)
		.addPropertyReadOnly("Stuck", &GetStuck)

		.addPropertyReadOnly("Instance", &GetModel)
		.addPropertyReadOnly("IsPlayer", &IsPlayer)

		//functions
		.addFunction("Init", &Init)
		.addFunction("InitAudio", &InitAudio)
		.addFunction("Reset", &Reset)
		.addFunction("ClearDamage", &ClearDamage)
		.addFunction("SetDrivable", &setDrivable, LUA_ARGS(bool, _def<int, 3>))
		
		.addFunction("GetCollider", &getColliderLua) 
		.addFunction("SetColliderID", &SetColliderID)
			
	.endClass();
}
