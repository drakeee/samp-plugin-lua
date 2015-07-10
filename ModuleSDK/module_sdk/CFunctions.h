#ifndef _FUNCTIONS_HEADER_
#define _FUNCTIONS_HEADER

#include <CArgumentReader.h>

extern "C"
{
#include <Include/Lua_351/lua.h>
}

class CFunctions
{
public:
	static int HelloWorld(lua_State* luaVM);
};

#endif