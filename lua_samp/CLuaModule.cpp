#include "CLuaModule.h"
#include "CCore.h"
#include "CUtility.h"

#ifndef WIN32
#include <algorithm>
#endif

CLuaModule::CLuaModule(std::string moduleName)
{
	m_modulePath = std::string(LUA_MODULES_FOLDER + moduleName);
	m_moduleName = moduleName;
#ifdef WIN32
	MultiByteToWideChar(0, 0, m_modulePath.c_str(), -1, m_fileName, 64);
#else
	m_fileName = m_modulePath;
#endif
	m_isInitialised = false;
}

CLuaModule::~CLuaModule()
{

}

int CLuaModule::_LoadModule()
{
	#ifdef WIN32
		m_Module = LoadLibrary(m_fileName);
		if (m_Module == NULL)
		{
			CUtility::printf("MODULE: Unable to load '%s' module!", m_moduleName.c_str());
			wprintf(L"MODULE: %s", m_fileName);
			return 1;
		}
	#else
		m_Module = dlopen(m_fileName.c_str(), RTLD_NOW);
		if (m_Module == NULL)
		{
			CUtility::printf("MODULE: Unable to load '%s' module!", m_moduleName.c_str());
			return 1;
		}
	#endif

	#ifdef WIN32
		m_initFunc = (INITFUNC)(GetProcAddress(m_Module, "InitModule"));
		if (m_initFunc == NULL)
		{
			CUtility::printf("MODULE: Unable to initialize '%s' module!", m_moduleName.c_str());
			return 2;
		}
	#else
		m_initFunc = (INITFUNC)(dlsym(m_Module, "InitModule"));
		if (dlerror() != NULL)
		{
			CUtility::printf("MODULE: Unable to initialize '%s' module!", m_moduleName.c_str());
			return 2;
		}
	#endif

	#ifdef WIN32
		m_FunctionInfo.DoPulse = (VOIDFUNC)(GetProcAddress(m_Module, "DoPulse"));
		if (m_FunctionInfo.DoPulse == NULL) return 3;
		m_FunctionInfo.RegisterFunctions = (LUAFUNC)(GetProcAddress(m_Module, "RegisterFunctions"));
		if (m_FunctionInfo.RegisterFunctions == NULL) return 4;

		m_FunctionInfo.ResourceStopping = (LUAFUNC)(GetProcAddress(m_Module, "ResourceStopping"));
	#else
		m_FunctionInfo.DoPulse = (VOIDFUNC)(dlsym(m_Module, "DoPulse"));
		if (m_FunctionInfo.DoPulse == NULL) return 3;
		m_FunctionInfo.RegisterFunctions = (LUAFUNC)(dlsym(m_Module, "RegisterFunctions"));
		if (m_FunctionInfo.RegisterFunctions == NULL) return 4;

		m_FunctionInfo.ResourceStopping = (LUAFUNC)(dlsym(m_Module, "ResourceStopping"));
	#endif
	
	if (!m_initFunc(this, &m_FunctionInfo.m_moduleName[0], &m_FunctionInfo.m_Author[0], &m_FunctionInfo.m_Version))
	{
		CUtility::printf("MODULE: Unable to initialize '%s' module!", m_moduleName.c_str());
		return 2;
	}

	CUtility::printf("MODULE: '%s' successfully loaded.", m_moduleName.c_str());
	CUtility::printf("- Module Name: '%s'", m_FunctionInfo.m_moduleName);
	CUtility::printf("- Author: '%s'", m_FunctionInfo.m_Author);
	CUtility::printf("- Version: '%g'\n", m_FunctionInfo.m_Version);

	m_isInitialised = true;
	return 3;
}

void CLuaModule::_RegisterFunctions(lua_State *L)
{
	m_FunctionInfo.RegisterFunctions(L);
}

void CLuaModule::_ResourceStopping(lua_State * luaVM)
{
	if (m_FunctionInfo.ResourceStopping)
		m_FunctionInfo.ResourceStopping(luaVM);
}

void CLuaModule::_DoPulse(void)
{
	m_FunctionInfo.DoPulse();
}

void CLuaModule::_UnregisterFunctions(void)
{
	std::map<lua_State *, CLuaManager *> *t = s_Core->GetLuaManagerContainer();
	for (auto it = (*t).begin(); it != (*t).end(); ++it)
	{
		lua_State* luaVM = it->first;
		auto iter = m_Functions.begin();
		for (; iter != m_Functions.end(); ++iter)
		{
			lua_pushnil(luaVM);
			lua_setglobal(luaVM, iter->c_str());
		}
	}
	m_Functions.clear();
}

void CLuaModule::ErrorPrintf(const char* szFormat, ...)
{
	char dest[512];
	va_list args;
	va_start(args, szFormat);
	vsprintf(dest, szFormat, args);
	va_end(args);

	CUtility::printf("ERROR: %s", dest);
}

void CLuaModule::DebugPrintf(lua_State * luaVM, const char* szFormat, ...)
{
	char dest[512];
	va_list args;
	va_start(args, szFormat);
	vsprintf(dest, szFormat, args);
	va_end(args);

	CUtility::printf("DEBUG: %s", dest);
}

void CLuaModule::Printf(const char* szFormat, ...)
{
	char dest[512];
	va_list args;
	va_start(args, szFormat);
	vsprintf(dest, szFormat, args);
	va_end(args);

	CUtility::printf(dest);
}

bool CLuaModule::RegisterFunction(lua_State * luaVM, const char *szFunctionName, lua_CFunction Func)
{
	if (luaVM)
	{
		if (szFunctionName)
		{
			lua_register(luaVM, szFunctionName, Func);
			if (std::find(m_Functions.begin(), m_Functions.end(), szFunctionName) == m_Functions.end())
			{
				m_Functions.push_back(szFunctionName);
			}
		}
	}
	else
	{
		CUtility::printf("MODULE: Lua is not initialised.");
	}
	return true;
}

bool CLuaModule::GetResourceName(lua_State * luaVM, std::string& strName)
{
	if (luaVM)
	{
		CResourceManager* p = s_Core->GetResourceManager(luaVM);
		if (p)
		{
			strName = p->GetResourceName();
			return true;
		}
	}
	return false;
}