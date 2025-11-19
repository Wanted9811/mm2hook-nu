#include "glowdata.h"

using namespace MM2;

dgBangerGlowData::dgBangerGlowData()
{
	this->Offset = Vector3(0.0f, 0.0f, 0.0f);
	this->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	this->Name[0] = NULL;
	this->Size = 1.5f;
	this->Index = 0;
	this->FlatMode = 0;
	this->FlatSize = Vector2(1.0f, 1.0f);
	this->FlatRotation = Vector3(0.0f, 0.0f, 0.0f);
	this->RotationSpeed = Vector3(0.0f, 0.0f, 0.0f);
	this->EnableFade = 0;
	this->FadeStyle = 0;
	this->FadeDirection = 2;
	this->InvFadeDirection = 0;
	this->FlashFreq = Vector2(0.0f, 0.0f);
	this->FlashFreq2 = Vector2(0.0f, 0.0f);
	this->InvFlashTime = 0;
	this->FlashPerIndex = 0;
	this->EnableShadow = 1;
	this->CustomShadow = 0;
	this->ShadowSize = 1.5f;
	this->ShadowIntensity = 1.0f;
}