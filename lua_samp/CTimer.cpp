#include "CTimer.h"
#include "CUtility.h"

namespace CTimer
{
	std::map<lua_State *, std::map<int, TimerStruct *>> timerContainer;
	bool eraseCalled = true;

	int CreateTimer(int interval, bool repeat, lua_State *L, int reference, CLuaArguments *luaArg)
	{
		TimerStruct *p = new TimerStruct(interval, repeat, L, reference, luaArg);
		p->nextCall = (GetTickCountEx() + interval);

		int timerid = GetFreeIndex(L);
		timerContainer[L][timerid] = p;

		return timerid;
	}

	int GetFreeIndex(lua_State *L)
	{
		unsigned int i = 0; // or whatever your smallest admissable key value is

		for (auto it = timerContainer[L].cbegin(), end = timerContainer[L].cend(); it != end && i == it->first; ++it, ++i){}

		return i;
	}

	bool DeleteTimer(int timerid, lua_State *L)
	{
		if (timerContainer.find(L) == timerContainer.end())
		{
			return false;
		}

		if (timerContainer[L].find(timerid) == timerContainer[L].end())
		{
			return false;
		}

		auto it = timerContainer[L].find(timerid);
		timerContainer[L].erase(it);

		return true;
	}

	void DeleteLuaTimers(lua_State *L)
	{
		auto it = timerContainer.find(L);
		if (it != timerContainer.end())
		{
			timerContainer.erase(it);
		}
	}

	void ExecuteTimer(lua_State *L)
	{
		auto mapTimer = timerContainer[L];
		for (auto mapIT = mapTimer.begin(); mapIT != mapTimer.end();)
		{
			TimerStruct * timer = timerContainer[L][mapIT->first];
			if (timer->nextCall < GetTickCountEx())
			{
				lua_State *L = timer->lua_VM;
				lua_rawgeti(timer->lua_VM, LUA_REGISTRYINDEX, timer->reference);

				auto *p = timer->luaArguments;
				auto tempVector = p->Get();
				for (auto arguments = tempVector.begin(); arguments != tempVector.end(); ++arguments)
				{
					switch ((*arguments)->GetType())
					{
					case LUA_TBOOLEAN:
					{
										lua_pushboolean(timer->lua_VM, (*arguments)->GetBoolean());
										break;
					}
					case LUA_TNUMBER:
					{
										lua_pushnumber(timer->lua_VM, (*arguments)->GetNumber());
										break;
					}
					case LUA_TSTRING:
					{
										lua_pushstring(timer->lua_VM, (*arguments)->GetString().c_str());
										break;
					}
					}
				}

				//CUtility::printf("Size: %d", tempVector.size());
				if (lua_pcall(timer->lua_VM, tempVector.size(), 0, 0) != 0)
				{
					CUtility::printf("setTimer error: %s", lua_tostring(timer->lua_VM, -1));
					lua_pop(timer->lua_VM, 1);
				}

				if (timer->repeat)
				{
					timer->nextCall = (GetTickCountEx() + timer->interval);
				}
				else
				{
					eraseCalled = true;
					mapIT = timerContainer[L].erase(mapIT);
				}
			}

			if (!eraseCalled)
				++mapIT;

			eraseCalled = false;
		}
	}

	void ProcessTick()
	{
		//CUtility::printf("Size: %d", timerContainer.size());
		for (auto mapIT = timerContainer.begin(); mapIT != timerContainer.end(); mapIT++)
		{
			ExecuteTimer(mapIT->first);
		}
	}
}