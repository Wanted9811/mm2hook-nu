#include "MMDMusicManagerHandler.h"

using namespace MM2;

void MMDMusicManagerHandler::UpdateMusic(float speed, int numCops, bool isAirBorne)
{
	auto musicManager = reinterpret_cast<MMDMusicManager*>(this);
	musicManager->UpdateMusic(speed, numCops, isAirBorne);
}

void MMDMusicManagerHandler::Install()
{
    InstallCallback("MMDMusicManager::UpdateMusic", "Updates cop chase music when many cops detect the player at the same time.",
        &UpdateMusic, {
            cb::call(0x41426E),
        }
    );
}