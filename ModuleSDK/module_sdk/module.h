#ifndef _MODULE_HEADER_
#define _MODULE_HEADER

#define MODULE_NAME		"Test module"
#define MODULE_AUTHOR	"Test"
#define MODULE_VERSION	1.0

#include <string.h>
#include "Include/Module/ILuaModuleManager.h"
#include "CFunctions.h"

#ifdef WIN32
#define LUAMODULE extern "C" __declspec(dllexport)
#else
#define LUAMODULE extern "C"
#endif

extern ILuaModuleManager *p_moduleManager;

#endif