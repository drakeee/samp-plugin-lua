#ifndef _RESOURCE_MANAGER_HEADER_
#define _RESOURCE_MANAGER_HEADER_

#include <map>
#include <vector>
#include <fstream>
#include "CLuaManager.h"
#include "CUtility.h"
#include "TinyXML/tinyxml2.h"

extern "C"
{
#include "Lua_351/lua.h"
#include "Lua_351/lualib.h"
#include "Lua_351/lauxlib.h"
}

class CResourceManager
{
private:
	bool						r_isRunning;
	bool						r_isActive;
	bool						r_isScriptsValid;
	std::string					r_resourceName;
	CLuaManager*				r_validScript;
	//std::map<std::string, std::vector<ReferenceStruct>> r_functionReferences;
	std::vector<CLuaManager *>	r_loadedScripts;
public:
	CResourceManager(const char* rN);
	~CResourceManager();

	std::string		GetResourceName(void) { return r_resourceName; }
	int				LoadResource(void);
	void			StartResource(void);
	void			StopResource(void);
	bool			IsResourceValid(void) { return r_isScriptsValid; }
	bool			IsValidFile(std::string);
	bool			IsRunning(void) { return r_isRunning; }
	CLuaManager*	GetLuaVM(void) { return r_validScript; }
};

#endif