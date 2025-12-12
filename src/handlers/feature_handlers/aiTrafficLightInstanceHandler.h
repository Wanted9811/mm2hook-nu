#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class aiTrafficLightInstanceHandler {
public:
	void DrawShadow();
	static void Reset();
	static void Install();
};