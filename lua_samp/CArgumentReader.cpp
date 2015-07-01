#include "CArgumentReader.h"

ArgReader::ArgReader(lua_State *L)
{
	lua_VM = L;
	argIndex = 1;
}

ArgReader::~ArgReader()
{

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
		intVariable = luaL_ref(lua_VM, LUA_REGISTRYINDEX);
		argIndex++;

		return;
	}

	argIndex++;
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