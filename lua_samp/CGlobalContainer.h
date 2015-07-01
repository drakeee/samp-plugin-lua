#ifndef _GLOBAL_CONTAINER_HEADER_
#define _GLOBAL_CONTAINER_HEADER_

#include <map>
#include <vector>
#include "CResourceManager.h"
#include "CLuaManager.h"

enum ArgumentType
{
	TYPE_NONE,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STRING
};

struct ReferenceStruct
{
	lua_State *lua_VM;
	std::string eventName;
	int reference;
};

namespace CContainer
{
	namespace CallbackArgsType
	{
		extern std::map<std::string, std::vector<int>> callbackArgumentsType;
	}

	namespace Resource
	{
		extern std::map<std::string, CResourceManager *> resourceContainer;
		void Add(CResourceManager* rM);
		void Delete(const char* resourceName);
		bool Check(const char* resourceName);
		CResourceManager *Get(const char* resourceName);
	}

	namespace LuaManager
	{
		extern std::map<lua_State *, CLuaManager *> luaResourceContainer;
		void Add(lua_State *lua_VM, CLuaManager *rM);
		void Delete(lua_State *lua_VM);
		CLuaManager *Get(lua_State *lua_VM);
	}

	namespace LuaResourceManager
	{
		extern std::map<lua_State *, CResourceManager *> luaResourceContainer;
		//extern std::vector<CLuaManager *> luaContainer;
		void Add(lua_State *lua_VM, CResourceManager *rM);
		void Delete(lua_State *lua_VM);
		CResourceManager *Get(lua_State *lua_VM);
		std::map<lua_State *, CResourceManager *> GetContainer();
	}

	namespace LuaReference
	{
		extern std::map<std::string, std::vector<ReferenceStruct>> luaReferences;
		void Add(lua_State *lua_VM, std::string eventName, int reference);
		void Delete(lua_State *lua_VM);
		std::map<std::string, std::vector<ReferenceStruct>> GetContainer(void);
	}
}

#endif