#include "aiAmbientVehicleAudioHandler.h"

using namespace MM2;

/*
    aiAmbientVehicleAudioHandler
*/

void aiAmbientVehicleAudioHandler::Init(char* basename, Vector3* position)
{
    auto vehicleAudio = reinterpret_cast<aiAmbientVehicleAudio*>(this);
    vehicleAudio->aiAmbientVehicleAudio::Init(basename, position);
}

void aiAmbientVehicleAudioHandler::Install() {
    InstallCallback("aiAmbientVehicleAudio::Init", "Use new Init function.",
        &Init, {
            cb::call(0x567F6D),
        }
    );
}