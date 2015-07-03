#include "CLuaArgument.h"
#include "CUtility.h"

CLuaArgument::CLuaArgument(lua_State *L, int argIndex)
{
	argType = lua_type(L, argIndex);
	switch (argType)
	{
		case LUA_TBOOLEAN:
		{
			argBoolean = !!lua_toboolean(L, argIndex);
			break;
		}
		case LUA_TNUMBER:
		{
			argNumber = lua_tonumber(L, argIndex);
			CUtility::printf("N: %d - %f", argIndex, argNumber);
			break;
		}
		case LUA_TSTRING:
		{
			argString = lua_tostring(L, argIndex);
			break;
		}

		default:
		{
			argType = 0;
			break;
		}
	}
}

CLuaArgument::~CLuaArgument()
{

}