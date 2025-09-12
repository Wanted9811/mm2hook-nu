#include "vehBreakableMgrHandler.h"

using namespace MM2;

/*
    vehBreakableMgrHandler
*/

static ConfigValue<bool> cfgBreakReflections("ReflectionsOnBreakables", true);

void vehBreakableMgrHandler::Install()
{
    vehBreakableMgr::EnableReflections = cfgBreakReflections.Get();
}