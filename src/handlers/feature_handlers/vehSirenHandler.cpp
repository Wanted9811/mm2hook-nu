#include "vehSirenHandler.h"

using namespace MM2;

/*
    vehSirenHandler
*/

vehSiren* vehSirenHandler::Construct()
{
    auto siren = reinterpret_cast<vehSiren*>(this);
	return new(siren) vehSiren();
}

void vehSirenHandler::Destruct()
{
    auto siren = reinterpret_cast<vehSiren*>(this);
    siren->vehSiren::~vehSiren();
}

void vehSirenHandler::Update()
{
    auto siren = reinterpret_cast<vehSiren*>(this);
    siren->vehSiren::Update();
}

void vehSirenHandler::Reset()
{
    auto siren = reinterpret_cast<vehSiren*>(this);
    siren->vehSiren::Reset();
}

void* vehSirenHandler::SizeOf()
{
    return hook::StaticThunk<0x577360>::Call<void*>(sizeof(vehSiren));
}

void vehSirenHandler::Install() {
    InstallCallback("vehSiren::Constructor", "Use our vehSiren constructor.",
        &Construct, {
            cb::call(0x42BE48),
        }
	);

    InstallCallback("vehSiren::Destructor", "Use our vehSiren destructor.",
        &Destruct, {
            cb::call(0x42BD43),
        }
	);

    InstallCallback("vehSiren::Update", "Use our vehSiren update.",
        &Update, {
            cb::call(0x42C920),
        }
    );

    InstallCallback("vehSiren::Reset", "Deactivates siren lights upon reset.",
        &Reset, {
            cb::call(0x42C3A6),
        }
    );

    InstallCallback("vehSiren::SizeOf", "Change size of vehSiren on vehicle initialization.",
        &SizeOf, {
            cb::call(0x42BE30),
        }
    );

    ConfigValue<float> cfgSirenRotationSpeed("SirenRotationSpeed", 3.1415927f);

    vehSiren::SirenRotationSpeed = cfgSirenRotationSpeed.Get();
}

