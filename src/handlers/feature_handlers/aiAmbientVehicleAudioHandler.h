#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class aiAmbientVehicleAudioHandler {
public:
	void Init(char* basename, MM2::Vector3* position);
	static void Install();
};