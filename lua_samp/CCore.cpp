#include "CCore.h"
#include "CUtility.h"

#ifndef WIN32
#include <algorithm>
#endif

CCore* s_Core;
CCore::CCore()
{
	s_ServerConfig = NULL;
	s_ServerConfig = new CServerConfig("server.cfg");
}

CCore::~CCore()
{

}

CServerConfig* CCore::GetConfig()
{
	return s_ServerConfig;
}

void CCore::LoadModules()
{
	for (int i = 0; i < s_ServerConfig->GetSettingSize("luamodules"); i++)
	{
		std::string moduleName = s_ServerConfig->GetSetting("luamodules", i);
		CUtility::printf("Loading module: %s", moduleName.c_str());
		LoadModule(moduleName);
	}
}

void CCore::LoadModule(std::string moduleName)
{
	bool IsModuleRunning = s_Core->IsModuleRunning(moduleName.c_str());
	if (!IsModuleRunning)
	{
		CLuaModule *luaModule = new CLuaModule(moduleName);
		luaModule->_LoadModule();

		if (luaModule->IsModuleValid())
		{
			//Register the resource pointer in the core object
			s_Core->RegisterModule(luaModule);
		}
	}
	else
	{
		CUtility::printf("MODULE: '%s' module is already running!", moduleName.c_str());
	}
}

void CCore::LoadResources()
{
	for (int i = 0; i < s_ServerConfig->GetSettingSize("luascripts"); i++)
	{
		std::string resourceName = s_ServerConfig->GetSetting("luascripts", i);
		if (resourceName.length())
		{
			CUtility::printf("Loading resource: %s", resourceName.c_str());
			LoadResource(resourceName);
		}
	}
}

bool CCore::LoadResource(std::string resourceName)
{
	bool IsResourceRunning = s_Core->IsResourceRunning(resourceName.c_str());
	if (!IsResourceRunning)
	{
		CResourceManager* resource = new CResourceManager(resourceName.c_str());
		resource->LoadResource();

		if (resource->IsResourceValid())
		{
			//Give some info to the user
			CUtility::printf("'%s' resource is successfully started.", resource->GetResourceName().c_str());

			//Start the resource
			resource->StartResource();

			//Register the resource pointer in the core object
			s_Core->RegisterResource(resource);

			return true;
		}
	}
	else
	{
		CUtility::printf("Unable to load '%s' resource! (resource is running)", resourceName.c_str());
	}

	return false;
}


void CCore::RegisterResource(CResourceManager* rM)
{
	resourceContainer[rM->GetResourceName()] = rM;
}

void CCore::DeleteResource(const char* resourceName)
{
	auto resource = resourceContainer.find(resourceName);
	if (resource == resourceContainer.end())
		return;

	resourceContainer.erase(resource);
}

CResourceManager *CCore::GetResourceByName(const char *resourceName)
{
	auto resource = resourceContainer.find(resourceName);
	if (resource == resourceContainer.end())
		return NULL;

	return resource->second;
}

bool CCore::IsResourceRunning(const char* resourceName)
{
	auto resource = resourceContainer.find(resourceName);
	if (resource == resourceContainer.end())
		return false;

	return true;
}

void CCore::RegisterLuaManager(lua_State *lua_VM, CLuaManager *rM)
{
	luaManagerContainer[lua_VM] = rM;
}

void CCore::DeleteLuaManager(lua_State *lua_VM)
{
	auto lua = luaManagerContainer.find(lua_VM);
	if (lua == luaManagerContainer.end())
		return;

	luaManagerContainer.erase(lua);
}

std::map<lua_State *, CLuaManager *>* CCore::GetLuaManagerContainer()
{
	return &luaManagerContainer;
}

CLuaManager *CCore::GetLuaManager(lua_State *lua_VM)
{
	return luaManagerContainer[lua_VM];
}

void CCore::RegisterLuaVM(lua_State *lua_VM, CResourceManager *rM)
{
	luaResourceContainer[lua_VM] = rM;
}

void CCore::DeleteLuaVM(lua_State *lua_VM)
{
	std::map<lua_State *, CResourceManager *>::iterator lua = luaResourceContainer.find(lua_VM);
	if (lua == luaResourceContainer.end())
		return;

	luaResourceContainer.erase(lua);
}

CResourceManager *CCore::GetResourceManager(lua_State *lua_VM)
{
	return luaResourceContainer[lua_VM];
}

std::map<lua_State *, CResourceManager *> *CCore::GetLuaResourceContainer()
{
	return &luaResourceContainer;
}

void CCore::RegisterReference(lua_State *lua_VM, std::string eventName, int reference)
{
	ReferenceStruct temp;
	temp.lua_VM = lua_VM;
	temp.eventName = eventName;
	temp.reference = reference;

	luaReferences[eventName].push_back(temp);
}

void CCore::DeleteReference(lua_State *lua_VM)
{
	for (std::map<std::string, std::vector<ReferenceStruct>>::iterator it = luaReferences.begin(); it != luaReferences.end(); ++it)
	{
		for (std::vector<ReferenceStruct>::iterator luaStruct = it->second.begin(); luaStruct != it->second.end();)
		{
			if (lua_VM == luaStruct->lua_VM)
			{
				//CUtility::printf("Copy");
				luaStruct = luaReferences[it->first].erase(luaStruct);
			}
			else
				++luaStruct;
		}
	}
}

std::map<std::string, std::vector<ReferenceStruct>> *CCore::GetReferenceContainer(void)
{
	return &luaReferences;
}

void CCore::RegisterModule(CLuaModule* module)
{
	if (std::find(moduleContainer.begin(), moduleContainer.end(), module) == moduleContainer.end())
	{
		moduleContainer.push_back(module);
	}
}

void CCore::DeleteModule(CLuaModule* module)
{
	for (auto iter = moduleContainer.begin(); iter != moduleContainer.end(); ++iter)
	{
		if (*iter == module)
		{
			moduleContainer.erase(iter);
			break;
		}
	}
}

bool CCore::IsModuleRunning(std::string moduleName)
{
	for (auto iter = moduleContainer.begin(); iter != moduleContainer.end(); ++iter)
	{
		if (!(*iter)->GetModuleName().compare(moduleName))
		{
			return true;
		}
	}

	return false;
}