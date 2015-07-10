#include "CFunctions.h"

int CFunctions::HelloWorld(lua_State *L)
{
	std::string helloSomebody;

	ArgReader argReader(L);
	argReader.ReadString(helloSomebody);

	lua_pushstring(L, helloSomebody.c_str());

	//number after the return represent how many values you want to return
	return 1;
}