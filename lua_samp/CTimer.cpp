#include "CTimer.h"
#include "CUtility.h"

namespace CTimer
{
	std::map<lua_State *, std::vector<TimerStruct *>> timerContainer;
	bool eraseCalled = true;

	int CreateTimer(int interval, bool repeat, lua_State *L, int reference, CLuaArguments *luaArg)
	{
		TimerStruct *p = new TimerStruct(interval, repeat, L, reference, luaArg);
		p->nextCall = (GetTickCount() + interval);
		timerContainer[L].push_back(p);

		return (timerContainer[L].size() - 1);
	}

	bool DeleteTimer(int timerid, lua_State *L)
	{
		if (timerid >= (int)timerContainer[L].size())
		{
			return false;
		}

		timerContainer[L].erase(timerContainer[L].begin() + timerid);
		return true;
	}

	void DeleteLuaTimers(lua_State *L)
	{
		auto it = timerContainer.find(L);
		timerContainer.erase(it);
	}

	void ExecuteTimer(lua_State *L)
	{
		std::vector<TimerStruct *> vec = timerContainer[L];
		for (auto it = vec.begin(); it != vec.end();)
		{
			if ((*it)->nextCall < GetTickCount())
			{
				lua_State *L = (*it)->lua_VM;
				lua_rawgeti((*it)->lua_VM, LUA_REGISTRYINDEX, (*it)->reference);

				auto *p = (*it)->luaArguments;
				auto tempVector = p->Get();
				for (auto arguments = tempVector.begin(); arguments != tempVector.end(); ++arguments)
				{
					switch ((*arguments)->GetType())
					{
					case LUA_TBOOLEAN:
					{
										 lua_pushboolean((*it)->lua_VM, (*arguments)->GetBoolean());
										 break;
					}
					case LUA_TNUMBER:
					{
										lua_pushnumber((*it)->lua_VM, (*arguments)->GetNumber());
										break;
					}
					case LUA_TSTRING:
					{
										lua_pushstring((*it)->lua_VM, (*arguments)->GetString().c_str());
										break;
					}
					}
				}

				//CUtility::printf("Size: %d", tempVector.size());
				if (lua_pcall((*it)->lua_VM, tempVector.size(), 0, 0) != 0)
				{
					CUtility::printf("setTimer error: %s", lua_tostring((*it)->lua_VM, -1));
					lua_pop((*it)->lua_VM, 1);
				}

				if ((*it)->repeat)
				{
					(*it)->nextCall = (GetTickCount() + (*it)->interval);
				}
				else
				{
					eraseCalled = true;
					it = timerContainer[L].erase(it);
				}
			}

			if (!eraseCalled)
				++it;

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