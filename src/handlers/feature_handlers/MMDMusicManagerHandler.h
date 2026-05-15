#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class MMDMusicManagerHandler {
public:
	void UpdateMusic(float speed, int numCops, bool isAirBorne);
	static void Install();
};