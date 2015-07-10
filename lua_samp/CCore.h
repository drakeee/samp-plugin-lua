#ifndef _CORE_HEADER_
#define _CORE_HEADER_

#include "CServerConfig.h"
#include "CResourceManager.h"
#include "CLuaModule.h"
#include "CLuaModule.h"
#include <vector>

struct ReferenceStruct
{
	lua_State *lua_VM;
	std::string eventName;
	int reference;
};

enum ArgumentType
{
	TYPE_NONE,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STRING
};

class CCore
{
public:
	//Core functions
	CCore();
	~CCore();
	CServerConfig*	GetConfig();
	void			LoadModules();
	void			LoadModule(std::string moduleName);
	void			LoadResources();
	bool			LoadResource(std::string resourceName);

	//Callback arguments
	inline void					RegisterCallback(std::string callbackName, std::initializer_list<int> argumentList) { callbackArgumentsType[callbackName].assign(argumentList.begin(), argumentList.end()); };
	inline std::vector<int>*	GetCallbackArguments(std::string callbackName)
	{
		auto it = callbackArgumentsType.find(callbackName);
		if (it != callbackArgumentsType.end())
			return &callbackArgumentsType[callbackName];

		return nullptr;
	}

	//Resource manager
	void				RegisterResource(CResourceManager* rM);
	void				DeleteResource(const char* resourceName);
	CResourceManager*	GetResourceByName(const char *resourceName);
	bool				IsResourceRunning(const char* resourceName);

	//Lua manager
	void									RegisterLuaManager(lua_State *lua_VM, CLuaManager *rM);
	void									DeleteLuaManager(lua_State *lua_VM);
	CLuaManager*							GetLuaManager(lua_State *lua_VM);
	std::map<lua_State *, CLuaManager *>*	GetLuaManagerContainer();

	//Get resource by lua virtual machine
	void										RegisterLuaVM(lua_State *lua_VM, CResourceManager *rM);
	void										DeleteLuaVM(lua_State *lua_VM);
	CResourceManager*							GetResourceManager(lua_State *lua_VM);
	std::map<lua_State *, CResourceManager *>*	GetLuaResourceContainer();

	//Lua reference container
	void													RegisterReference(lua_State *lua_VM, std::string eventName, int reference);
	void													DeleteReference(lua_State *lua_VM);
	std::map<std::string, std::vector<ReferenceStruct>>*	GetReferenceContainer(void);

	//Lua module manager
	void								RegisterModule(CLuaModule* module);
	void								DeleteModule(CLuaModule* module);
	inline std::vector<CLuaModule *>*	GetModuleContainer() { return &moduleContainer; }
	bool								IsModuleRunning(std::string moduleName);
private:
	CServerConfig*										s_ServerConfig;
	std::map<std::string, std::vector<int>>				callbackArgumentsType;
	std::map<std::string, CResourceManager *>			resourceContainer;
	std::map<lua_State *, CLuaManager *>				luaManagerContainer;
	std::map<lua_State *, CResourceManager *>			luaResourceContainer;
	std::map<std::string, std::vector<ReferenceStruct>> luaReferences;
	std::vector<CLuaModule *>							moduleContainer;
};

extern CCore* s_Core;
#endif