#ifndef _ARGUMENT_READER_HEADER_
#define _ARGUMENT_READER_HEADER_

#include <string>
extern "C"
{
#include "Include/Lua_351/lua.h"
#include "Include/Lua_351/lauxlib.h"
}

class ArgReader
{
public:
	ArgReader(lua_State *L, int stackStart = 1);
	~ArgReader();

	inline bool	IsBool(void) { return (lua_type(lua_VM, argIndex) == LUA_TBOOLEAN); }
	inline bool	IsNumber(void) { return (lua_type(lua_VM, argIndex) == LUA_TNUMBER); }
	inline bool	IsString(void) { return (lua_type(lua_VM, argIndex) == LUA_TSTRING); }
	inline bool IsNil(void) { return (lua_type(lua_VM, argIndex) == LUA_TNIL); }

	void		ReadLuaNumber(lua_Number& numberVariable, int defaultValue = NULL);
	void		ReadNumber(int& intVariable, int defaultValue = NULL);
	void		ReadFloat(float& floatVariable, float defaultValue = NULL);
	void		ReadBool(bool& boolVariable, bool defaultValue = false);
	void		ReadFunction(int& intVariable);
	void		ReadFunctionComplete();
	void		ReadString(std::string& stringVariable, const char* defaultValue = NULL);

	lua_State*	lua_VM;
	int			argIndex;
	int*		pendingFunctionRefOut;
	int			pendingFunctionRef;
};

#endif