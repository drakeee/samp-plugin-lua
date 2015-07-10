#ifndef __ILUAMODULE_MANAGER_HEADER_
#define __ILUAMODULE_MANAGER_HEADER_

#define MAX_INFO_LENGTH 128

extern "C"
{
#include "Include/Lua_351/lua.h"
#include "Include/Lua_351/lualib.h"
#include "Include/Lua_351/lauxlib.h"
}
#include <string>

class ILuaModuleManager
{
public:
	virtual void                ErrorPrintf(const char* szFormat, ...) = 0;
	virtual void                DebugPrintf(lua_State * luaVM, const char* szFormat, ...) = 0;
	virtual void                Printf(const char* szFormat, ...) = 0;

	virtual bool                RegisterFunction(lua_State * luaVM, const char *szFunctionName, lua_CFunction Func) = 0;
	virtual bool                GetResourceName(lua_State * luaVM, std::string &strName) = 0;
};

#endif