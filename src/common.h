//
// Common resources
//

#pragma once

#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdarg.h>

#include <vector>
#include <map>

#include <Windows.h>

/*
    Yeah because it's not like GetClassName would ever be used anywhere...*stares at Microsoft*
*/
#ifdef GetClassName
#define WINUSER_GetClassName GetClassName
#undef GetClassName
#endif

#ifdef LoadBitmap
#undef LoadBitmap
#endif

#include "settings.h"

#include "util.h"
#include "logfile.h"
#include "console.h"

#ifdef _LUA_EMBED
// Lua 5.3.2 embedded
# include <lua/lua.hpp>
# ifndef NO_LUAINTF
#  undef LUAINTF_AUTO_DOWNCAST
#  define LUAINTF_AUTO_DOWNCAST 0
#  include <LuaIntf/LuaIntf.h>
using namespace LuaIntf;
LUA_USING_LIST_TYPE(std::vector)
LUA_USING_MAP_TYPE(std::map)
# endif // !NO_LUAINTF
#endif // !_LUA_EMBED

//  C4091: '__declspec(novtable)': ignored on left of 'x' when no variable is declared
#pragma warning (disable : 4091)
// disable warning/error about __identifier(<string>)
#pragma warning(disable:4483)

// Unsigned type definitions
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long uint64;