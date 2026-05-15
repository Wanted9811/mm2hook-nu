#include "aiPathHandler.h"

using namespace MM2;

/*
    aiPathHandler
*/

void aiPathHandler::UpdatePedestrians()
{
    auto path = reinterpret_cast<aiPath*>(this);
    aiPedestrianHandler::UpdateAttempts = 0;

    path->UpdatePedestrians();
}

void aiPathHandler::UpdateAmbients()
{
    auto path = reinterpret_cast<aiPath*>(this);
    if (path != nullptr)
        path->UpdateAmbients();
}

void aiPathHandler::Install() {
    InstallCallback("aiPath::UpdatePedestrians", "Limits the number of update attempts for pedestrians.",
        &UpdatePedestrians, {
            cb::call(0x536FE0), // aiMap::Update
        }
    );

    InstallCallback("aiPath::UpdateAmbients", "Fixes a random crash that happens during updating ambients.",
        &UpdateAmbients, {
            cb::call(0x536F83), // aiMap::Update
        }
    );
}