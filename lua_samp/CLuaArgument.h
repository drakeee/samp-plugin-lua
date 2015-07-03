#ifndef _LUA_ARGUMENT_HEADER_
#define _LUA_ARGUMENT_HEADER_

#include <string>
extern "C"
{
#include "Lua_351/lua.h"
}

class CLuaArgument
{
public:
	CLuaArgument(lua_State *L, int argIndex);
	~CLuaArgument();
	inline int GetType() { return argType; }
	inline bool GetBoolean() { return argBoolean; }
	inline lua_Number GetNumber() { return argNumber; }
	inline std::string GetString() { return argString; }
private:
	int argType;
	bool argBoolean;
	lua_Number argNumber;
	std::string argString;
};

#endif