#include "camPovCSHandler.h"

using namespace MM2;

void camPovCSHandler::UpdatePOV()
{
	auto camPov = reinterpret_cast<camPovCS*>(this);
	camPov->UpdatePOV();
}

void camPovCSHandler::Install() {
	InstallCallback("camPovCS::UpdatePOV", "Implements cam wobble feature.",
		&UpdatePOV, {
			cb::call(0x51D573),
		}
	);
}