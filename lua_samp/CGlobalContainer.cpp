#include "CGlobalContainer.h"

namespace CContainer
{
	namespace CallbackArgsType
	{
		std::map<std::string, std::vector<int>> callbackArgumentsType;
	}

	namespace Resource
	{
		std::map<std::string, CResourceManager *> resourceContainer;
		void Add(CResourceManager* rM)
		{
			resourceContainer[rM->GetResourceName()] = rM;
		}

		void Delete(const char* resourceName)
		{
			std::map<std::string, CResourceManager *>::iterator resource = resourceContainer.find(resourceName);
			if (resource == resourceContainer.end())
				return;

			resourceContainer.erase(resource);
		}

		CResourceManager *Get(const char *resourceName)
		{
			std::map<std::string, CResourceManager *>::iterator resource = resourceContainer.find(resourceName);
			if (resource == resourceContainer.end())
				return NULL;

			return resource->second;
		}

		bool Check(const char* resourceName)
		{
			std::map<std::string, CResourceManager *>::iterator resource = resourceContainer.find(resourceName);
			if (resource == resourceContainer.end())
				return false;

			return true;
			//return std::find_if(resourceContainer.begin(), resourceContainer.end(), std::bind2nd(SearchResourceManager(), rM->GetResourceName.c_str())) == resourceContainer.end();
			//return (std::find(resourceContainer.begin(), resourceContainer.end(), rM) != resourceContainer.end());
		}
	}

	namespace LuaManager
	{
		std::map<lua_State *, CLuaManager *> luaManagerContainer;
		void Add(lua_State *lua_VM, CLuaManager *rM)
		{
			luaManagerContainer[lua_VM] = rM;
		}

		void Delete(lua_State *lua_VM)
		{
			std::map<lua_State *, CLuaManager *>::iterator lua = luaManagerContainer.find(lua_VM);
			if (lua == luaManagerContainer.end())
				return;

			luaManagerContainer.erase(lua);
		}

		CLuaManager *Get(lua_State *lua_VM)
		{
			return luaManagerContainer[lua_VM];
		}
	}

	namespace LuaResourceManager
	{
		std::map<lua_State *, CResourceManager *> luaResourceContainer;
		void Add(lua_State *lua_VM, CResourceManager *rM)
		{
			luaResourceContainer[lua_VM] = rM;
		}

		void Delete(lua_State *lua_VM)
		{
			std::map<lua_State *, CResourceManager *>::iterator lua = luaResourceContainer.find(lua_VM);
			if (lua == luaResourceContainer.end())
				return;

			luaResourceContainer.erase(lua);
		}

		CResourceManager *Get(lua_State *lua_VM)
		{
			return luaResourceContainer[lua_VM];
		}

		std::map<lua_State *, CResourceManager *> GetContainer()
		{
			return luaResourceContainer;
		}
	}

	namespace LuaReference
	{
		std::map<std::string, std::vector<ReferenceStruct>> luaReferences;
		void Add(lua_State *lua_VM, std::string eventName, int reference)
		{
			ReferenceStruct temp;
			temp.lua_VM = lua_VM;
			temp.eventName = eventName;
			temp.reference = reference;

			luaReferences[eventName].push_back(temp);
		}

		void Delete(lua_State *lua_VM)
		{
			for (std::map<std::string, std::vector<ReferenceStruct>>::iterator it = luaReferences.begin(); it != luaReferences.end(); ++it)
			{
				for (std::vector<ReferenceStruct>::iterator luaStruct = it->second.begin(); luaStruct != it->second.end();)
				{
					if (lua_VM == luaStruct->lua_VM)
					{
						//CUtility::printf("Copy");
						luaStruct = luaReferences[it->first].erase(luaStruct);
					} else
						++luaStruct;
				}
			}
		}

		std::map<std::string, std::vector<ReferenceStruct>> GetContainer(void)
		{
			return luaReferences;
		}
	}
}