#include "CSampCallbacks.h"
#include <algorithm>
#include <sstream>

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandText(int playerid, const char *cmdtext)
{
	int playerID = (int)playerid;
	std::string commandWithArg(cmdtext);
	commandWithArg.erase(0, 1); //cut the slash from the beginning

	std::stringstream ss(commandWithArg); // turn the string into a stream.
	std::string commandName;
	getline(ss, commandName, ' '); //get the commandname apart from the args
	commandName.insert(0, "OPCT_");

	std::map<std::string, std::vector<ReferenceStruct>> temp = CContainer::LuaReference::GetContainer();
	std::vector<ReferenceStruct> a = temp[commandName];
	for (std::vector<ReferenceStruct>::iterator it = a.begin(); it != a.end(); ++it)
	{
		std::string tok;

		lua_State *L = it->lua_VM;
		lua_rawgeti(L, LUA_REGISTRYINDEX, it->reference);
		lua_pushnumber(L, playerID);

		int argNum = 1;

		while (getline(ss, tok, ' '))
		{
			lua_pushstring(L, tok.c_str());
			argNum++;
		}

		if (lua_pcall(L, argNum, LUA_MULTRET, 0) != 0)
		{
			CUtility::printf("Error calling LUA callback (onPlayerCommandText)");
			CUtility::printf("-- %s", lua_tostring(L, -1));
			lua_pop(L, 1);
		}

		return 1;
	}

	return 0;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPublicCall(AMX *amx, const char *name, cell *params, cell *retval)
{
	//handle OnPlayerCommandText on other callback
	if (!strcmp(name, "OnPlayerCommandText"))
		return true;

	//get the number of arguments
	int argc = (params[0] / sizeof(cell));

	//format callback first letter to lowercase
	std::string callbackName(name);
	callbackName[0] = std::tolower(callbackName[0]);

	std::map<std::string, std::vector<ReferenceStruct>> temp = CContainer::LuaReference::GetContainer();
	std::vector<ReferenceStruct> a = temp[callbackName];
	for (std::vector<ReferenceStruct>::iterator it = a.begin(); it != a.end(); ++it)
	{
		lua_State *L = it->lua_VM;
		lua_rawgeti(L, LUA_REGISTRYINDEX, it->reference);

		std::vector<int> argsType = CContainer::CallbackArgsType::callbackArgumentsType[callbackName];

		int numArg = 1;
		for (std::vector<int>::iterator argType = argsType.begin(); argType != argsType.end(); ++argType)
		{
			switch (*argType)
			{
				case ArgumentType::TYPE_INT:
				{
					//CUtility::printf("Push number: %d", params[numArg]);
					lua_pushnumber(L, params[numArg]);
					break;
				}
				case ArgumentType::TYPE_FLOAT:
				{
					//CUtility::printf("Push float: %f", params[numArg]);
					char buff[32];
					sprintf(buff, "%f", params[numArg]);

					lua_pushstring(L, buff);
					break;
				}
				case ArgumentType::TYPE_STRING:
				{
					char* buff = NULL;
					amx_StrParam(amx, params[1], buff);

					//CUtility::printf("Push string: %s", buff);
					lua_pushstring(L, buff);
					break;
				}
			}
		}

		if (lua_pcall(L, argc, LUA_MULTRET, 0) != 0)
		{
			CUtility::printf("Error calling LUA callback (%s)", callbackName.c_str());
			CUtility::printf("-- %s", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnRconCommand(const char* cmd)
{
	std::stringstream stream(cmd);
	std::string command, res;
	stream >> command >> res;

	if (!strcmp(command.c_str(), "start"))
	{
		if (res.length() == 0)
		{
			CUtility::printf("USAGE: start <resource name>");
			return true;
		}

		bool IsResourceRunning = CContainer::Resource::Check(res.c_str());
		if (!IsResourceRunning)
		{
			CResourceManager* resource = new CResourceManager(res.c_str());
			resource->LoadResource();

			if (resource->IsResourceValid())
			{
				CUtility::printf("'%s' resource is successfully started.", resource->GetResourceName().c_str());
				resource->StartResource();
				CContainer::Resource::Add(resource);
			}
		}
		else
		{
			CUtility::printf("Unable to load '%s' resource! (resource is running)", res.c_str());
		}

		return true;
	}

	if (!strcmp(command.c_str(), "stop"))
	{
		if (res.length() == 0)
		{
			CUtility::printf("USAGE: stop <resource name>");
			return true;
		}

		bool IsResourceRunning = CContainer::Resource::Check(res.c_str());
		if (IsResourceRunning)
		{
			CResourceManager *resource = CContainer::Resource::Get(res.c_str());
			//CUtility::printf("IsRunning: %s", resource->GetResourceName().c_str());
			resource->StopResource();
		}
		else
		{
			CUtility::printf("Resource '%s' is not running!", res.c_str());
		}

		return true;
	}

	//CUtility::printf("Parancs: %s", cmd);
	return true;
}
