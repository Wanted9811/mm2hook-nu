#include <modules\banger.h>
using namespace MM2;

/*
    dgBangerDataManager
*/
declfield(dgBangerDataManager::Instance)(0x627670);

/*
    dgBangerManager
*/
declfield(dgBangerManager::Instance)(0x627678);

/*
    dgBangerActiveManager
*/
declfield(dgBangerActiveManager::Instance)(0x627630);

/*
    dgBangerInstance
*/
declfield(dgBangerInstance::sm_RefAlpha)(0x5C55F4);
declfield(dgBangerInstance::DefaultGlowTexture)(0x62767C);

bool dgBangerInstance::GlowLoaded = false;
gfxTexture* dgBangerInstance::RedGlowTexture = NULL;