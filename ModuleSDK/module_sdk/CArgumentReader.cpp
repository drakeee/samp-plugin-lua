#include "CArgumentReader.h"
#include "module.h"

ArgReader::ArgReader(lua_State *L, int stackStart)
{
	lua_VM = L;
	argIndex = stackStart;
	pendingFunctionRefOut = NULL;
	pendingFunctionRef = -1;
}

ArgReader::~ArgReader()
{

}

void ArgReader::ReadLuaNumber(lua_Number& numberVariable, int defaultValue)
{
	//check if argument is number or string
	int argType = lua_type(lua_VM, argIndex);
	if (argType == LUA_TNUMBER || argType == LUA_TSTRING)
	{
		//read if it is
		numberVariable = lua_tonumber(lua_VM, argIndex);
		argIndex++;

		return;
	}
	else
	{
		//if it has default value
		if (defaultValue != NULL)
		{
			//then just fill it with them
			numberVariable = defaultValue;
			argIndex++;

			return;
		}
	}

	numberVariable = 0;
	argIndex++;
}

void ArgReader::ReadNumber(int& intVariable, int defaultValue)
{
	//check if argument is number or string
	int argType = lua_type(lua_VM, argIndex);
	if (argType == LUA_TNUMBER || argType == LUA_TSTRING)
	{
		//read if it is
		int readNumber = (int)lua_tonumber(lua_VM, argIndex);

		//fill the variable and increase argindex
		intVariable = readNumber;
		argIndex++;

		return;
	}
	else
	{
		//if it has default value
		if (defaultValue != NULL)
		{
			//then just fill it with them
			intVariable = defaultValue;
			argIndex++;

			return;
		}
	}

	intVariable = 0;
	argIndex++;
}

void ArgReader::ReadFloat(float& floatVariable, float defaultValue)
{
	//check if argument is number or string
	int argType = lua_type(lua_VM, argIndex);
	if (argType == LUA_TNUMBER || argType == LUA_TSTRING)
	{
		//read if it is
		float readFloat = (float)lua_tonumber(lua_VM, argIndex);

		//fill the variable and increase argindex
		floatVariable = readFloat;
		argIndex++;

		return;
	}
	else
	{
		//if it has default value
		if (defaultValue != NULL)
		{
			//then just fill it with them
			floatVariable = defaultValue;
			argIndex++;

			return;
		}
	}

	floatVariable = 0.0;
	argIndex++;
}

void ArgReader::ReadBool(bool& boolVariable, bool defaultValue)
{
	//check if argument is boolean
	int argType = lua_type(lua_VM, argIndex);
	if (argType == LUA_TBOOLEAN)
	{
		//read if it is and convert it to boolean, then increase argindex
		boolVariable = !!lua_toboolean(lua_VM, argIndex);
		argIndex++;

		return;
	}
	else
	{
		//if it has default value
		if (defaultValue != NULL)
		{
			//fill it with them then increase argindex
			boolVariable = defaultValue;
			argIndex++;

			return;
		}
	}

	//if everything else fall just stick with false value
	boolVariable = false;
	argIndex++;
}

void ArgReader::ReadFunction(int& intVariable)
{
	int argType = lua_type(lua_VM, argIndex);
	if (argType == LUA_TFUNCTION)
	{
		//lua_pushvalue(lua_VM, -1);
		//intVariable = luaL_ref(lua_VM, LUA_REGISTRYINDEX);
		pendingFunctionRefOut = &intVariable; //thanks to MTA
		pendingFunctionRef = argIndex;
		argIndex++;

		return;
	}

	argIndex++;
}

void ArgReader::ReadFunctionComplete()
{
	if (pendingFunctionRef == -1)
	{
		p_moduleManager->Printf("CArgumentReader::ReadFunctionComplete called before CArgumentReader::ReadFunction.");
		return;
	}

	lua_settop(lua_VM, pendingFunctionRef);
	int ref = luaL_ref(lua_VM, LUA_REGISTRYINDEX);

	*pendingFunctionRefOut = ref;
	pendingFunctionRef = -1;
}

void ArgReader::ReadString(std::string& stringVariable, const char* defaultValue)
{
	//check if argument is string
	int argType = lua_type(lua_VM, argIndex);
	if (argType == LUA_TNUMBER || argType == LUA_TSTRING)
	{
		//if string then append to the string variable, then increase argument index
		stringVariable.append(lua_tostring(lua_VM, argIndex));
		argIndex++;

		return;
	}
	else if (argType == LUA_TNIL || argType == LUA_TNONE)
	{
		//if default value is null just return
		if (defaultValue == NULL)
			return;

		//if not then append the value to the string, then increase argument index
		stringVariable.append(defaultValue);
		argIndex++;

		return;
	}

	//if not string, or is it not nil then just clear the variable, then increase argument index
	stringVariable = "";
	argIndex++;
}