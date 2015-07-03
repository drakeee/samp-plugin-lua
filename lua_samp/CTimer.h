#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include <map>
#include "CLuaArguments.h"
extern "C"
{
#include "Lua_351/lua.h"
}

struct TimerStruct
{
	int nextCall;
	int interval;
	bool repeat;
	lua_State *lua_VM;
	int reference;
	CLuaArguments *luaArguments;
	TimerStruct(int interval, bool repeat, lua_State *L, int reference, CLuaArguments *luaArguments) : interval(interval), repeat(repeat), lua_VM(L), reference(reference), luaArguments(luaArguments){}
};

namespace CTimer
{
	extern std::map<lua_State *, std::vector<TimerStruct *>> timerContainer;
	//extern std::vector<TimerStruct *> timerContainer;
	extern bool eraseCalled;

	int CreateTimer(int interval, bool repeat, lua_State *L, int reference, CLuaArguments *luaArg);
	bool DeleteTimer(int timerid, lua_State *L);
	void DeleteLuaTimers(lua_State *L);
	void ExecuteTimer(lua_State *L);
	void ProcessTick();
}

#endif