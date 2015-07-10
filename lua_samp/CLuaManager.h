#ifndef _LUA_MANAGER_HEADER_
#define _LUA_MANAGER_HEADER_

#define LUA_MODULES_FOLDER			"./lua/modules/"
#define LUA_RESOURCES_FOLDER		"./lua/resources/"

#include <map>
#include <vector>
#include "CUtility.h"
#include "CLuaFunctions.h"

extern "C"
{
#include "Lua_351/lua.h"
#include "Lua_351/lualib.h"
#include "Lua_351/lauxlib.h"
}

class CLuaManager
{
private:
	std::string					resourceName;
	std::string					fileName;
	lua_State*					lua_VM;
	std::vector<std::string>	fileContainer;
	//std::map<char *, std::vector<ReferenceStruct>> luaReferences;
public:
	CLuaManager(std::string rName);
	~CLuaManager();
	void				InitVM(void);
	void				StartLua(void);
	void				DisableFunctions(lua_State *L);
	void				RegisterPreScript(lua_State *L);
	void				RegisterModuleFunctions(lua_State *L);
	void				RegisterFunctions(lua_State *L);
	void				RegisterMacros(lua_State *L);
	void				CallInitExit(const char* scriptInit);
	void				AddFile(std::string fileName) { fileContainer.push_back(fileName); };
	std::string			GetResourceName() { return resourceName; }
	lua_State*			GetVirtualMachine(void) { return lua_VM; }
	//void RegisterFunctionReference(std::string eventName, int reference);
};

#endif