#ifndef _LUA_MODULE_HEADER
#define _LUA_MODULE_HEADER
#define MAX_INFO_LENGTH	128

#include <string>
#include <vector>
#include "ILuaModuleManager.h"
#include <Windows.h>

extern "C"
{
#include "Lua_351/lua.h"
}

typedef bool (*VOIDFUNC)	(void);
typedef void (*LUAFUNC)		(lua_State *);
typedef bool (*INITFUNC)	(ILuaModuleManager *, char *, char *, float *);

struct FunctionInfo
{
	// module information
	char				m_moduleName[MAX_INFO_LENGTH];
	char				m_Author[MAX_INFO_LENGTH];
	float				m_Version;
	std::string			m_fileName;

	// module function pointers
	VOIDFUNC	ShutdownModule;
	VOIDFUNC	DoPulse;
	LUAFUNC		RegisterFunctions;

	LUAFUNC		ResourceStopping;
	LUAFUNC		ResourceStopped;
};

class CLuaModule : public ILuaModuleManager
{
private:
	std::string		m_modulePath;
	std::string		m_moduleName;
	char			m_teszt[MAX_INFO_LENGTH];
	wchar_t			m_fileName[64];
	bool			m_isInitialised;
	HMODULE			m_Module;
	INITFUNC		m_initFunc;
	FunctionInfo	m_FunctionInfo;

	std::vector<std::string> m_Functions;
public:
	CLuaModule(std::string moduleName);
	~CLuaModule();

	int					_LoadModule();
	void				_RegisterFunctions(lua_State *L);
	void				_ResourceStopping(lua_State *L);
	void				_DoPulse(void);
	void				_UnregisterFunctions(void);
	inline std::string	GetModuleName() { return m_moduleName; }
	inline bool			IsModuleValid() { return m_isInitialised; }

	void                ErrorPrintf(const char* szFormat, ...);
	void                DebugPrintf(lua_State * luaVM, const char* szFormat, ...);
	void                Printf(const char* szFormat, ...);

	bool	RegisterFunction(lua_State *L, const char* functionName, lua_CFunction func);
	bool	GetResourceName(lua_State * luaVM, std::string& strName);
};

#endif