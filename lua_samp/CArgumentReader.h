#ifndef _ARGUMENT_READER_HEADER_
#define _ARGUMENT_READER_HEADER_

#include <string>
#include "CLuaArguments.h"
extern "C"
{
#include "Lua_351/lua.h"
#include "Lua_351/lauxlib.h"
}

class ArgReader
{
public:
	ArgReader(lua_State *L);
	~ArgReader();

	void ReadNumber(int& intVariable, int defaultValue = NULL);
	void ReadFloat(float& floatVariable, float defaultValue = NULL);
	void ReadBool(bool& boolVariable, bool defaultValue = false);
	void ReadArguments(CLuaArguments &luaArgumentsVariable);
	void ReadFunction(int& intVariable);
	void ReadFunctionComplete();
	void ReadString(std::string& stringVariable, const char* defaultValue = NULL);

	lua_State *lua_VM;
	int argIndex;
	int *pendingFunctionRefOut;
	int pendingFunctionRef;
};

#endif