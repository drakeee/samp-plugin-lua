#include "module.h"

ILuaModuleManager *p_moduleManager = NULL;

LUAMODULE bool InitModule(ILuaModuleManager *moduleManager, char *moduleName, char *moduleAuthor, float *moduleVersion)
{
	p_moduleManager = moduleManager;

	//set some info about the module
	strncpy(moduleName,		MODULE_NAME, MAX_INFO_LENGTH);
	strncpy(moduleAuthor,	MODULE_AUTHOR, MAX_INFO_LENGTH);
	(*moduleVersion) =		MODULE_VERSION;

	return true;
}

LUAMODULE void RegisterFunctions(lua_State * luaVM)
{
	p_moduleManager->RegisterFunction(luaVM, "helloWorld", CFunctions::HelloWorld);
}


LUAMODULE bool DoPulse(void)
{
	return true;
}

LUAMODULE bool ResourceStopping(lua_State * luaVM)
{
	return true;
}