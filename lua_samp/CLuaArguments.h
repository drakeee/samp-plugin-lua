#ifndef _LUA_ARGUMENTS_HEADER_
#define _LUA_ARGUMENTS_HEADER_

#include "CLuaArgument.h"
#include <vector>

class CLuaArguments
{
public:
	CLuaArguments();
	~CLuaArguments();
	void Add(CLuaArgument *);
	std::vector<CLuaArgument *> Get(void) { return argumentsContainer; }
private:
	std::vector<CLuaArgument *> argumentsContainer;
	int teszt;
};

#endif