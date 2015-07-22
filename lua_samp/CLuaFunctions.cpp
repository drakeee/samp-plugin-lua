#include "CLuaFunctions.h"
#include "CCore.h"
#include "CTimer.h"

LUA_FUNCTION DisableFunction(lua_State *L)
{
	CUtility::printf("This function is disabled for some reason.");
	lua_pushboolean(L, false);

	return 1;
}

void stackdump(lua_State* l)
{
	int argn = lua_gettop(l);

	printf("total in stack %d\n", argn);

	//stacks starts from 1 and end at argn
	for (int i = 1; i <= argn; i++)
	{
		//get type at stack index
		int t = lua_type(l, i);

		switch (t)
		{
		case LUA_TSTRING:
		{
							printf("%d. - string: '%s'\n", i, lua_tostring(l, i));
							break;
		}
		case LUA_TBOOLEAN:
		{
							 printf("%d. - boolean %s\n", i, (lua_toboolean(l, i) ? "true" : "false"));
							 break;
		}
		case LUA_TNUMBER:
		{
							printf("%d. - number: %g\n", i, lua_tonumber(l, i));
							break;
		}
		default:
		{
				   printf("%d. - %s\n", i, lua_typename(l, t));
				   break;
		}
		}
	}
	printf("\n");
}

LUA_FUNCTION isResourceRunning(lua_State *L)
{
	std::string resourceName;

	ArgReader argReader(L);
	argReader.ReadString(resourceName);

	//get resource by name
	CResourceManager* p = s_Core->GetResourceByName(resourceName.c_str());
	if (p == NULL)
	{
		lua_pushboolean(L, false);
	}
	else
	{
		lua_pushboolean(L, true);
	}

	return 1;
}

LUA_FUNCTION tocolor(lua_State *L)
{
	int32_t color32;
	lua_Number color64;

	ArgReader argReader(L);
	int argn = lua_gettop(L);
	if (argn > 1)
	{
		int r, g, b, a;
		argReader.ReadNumber(r);
		argReader.ReadNumber(g);
		argReader.ReadNumber(b);

		if (argn == 4)
		{
			argReader.ReadNumber(a);
			color32 = ((r << 24) + (g << 16) + (b << 8) + (a));
		}
		else
			color32 = ((r << 16) + (g << 8) + (b));
	}
	else
	{
		//because Lua is in 64 bit mode, we need to convert it back to 32 bit
		argReader.ReadLuaNumber(color64);
		color32 = (int)((int64_t)color64 - (((int64_t)color64 >> 32) << 32));
	}

	lua_pushnumber(L, (int)color32);

	return 1;
}

LUA_FUNCTION call(lua_State *L)
{
	int argn = lua_gettop(L);
	if (argn < 2)
		return CUtility::printf("call function got less, than 2 argument."), 0;

	std::string resourceName;
	std::string functionName;

	ArgReader argReader(L);
	argReader.ReadString(resourceName);
	argReader.ReadString(functionName);

	//get resource by name
	CResourceManager* p = s_Core->GetResourceByName(resourceName.c_str());
	if (p == NULL)
	{
		//if null then inform the user that there is no resource
		CUtility::printf("No resource running under '%s' name!", resourceName.c_str());
		return 0;
	}

	//get resource LuaVM
	lua_State *lua_VM = p->GetLuaVM()->GetVirtualMachine();
	
	//save how many values have the stack
	int top = lua_gettop(lua_VM);

	//call the function
	lua_getglobal(lua_VM, functionName.c_str());

	//loop through the arguments and pass the arguments to the function
	for (int i = 3; i <= argn; i++)
	{
		//loop through the arguments and push to the stack
		int iType = lua_type(L, i);
		switch (iType)
		{
		case LUA_TNUMBER:
		{
							lua_Number tempNumber;
							argReader.ReadLuaNumber(tempNumber);
							lua_pushnumber(lua_VM, tempNumber);
							break;
		}
		case LUA_TBOOLEAN:
		{
							 bool tempBool;
							 argReader.ReadBool(tempBool);
							 lua_pushboolean(lua_VM, tempBool);
							 break;
		}
		case LUA_TSTRING:
		{
							std::string tempString;
							argReader.ReadString(tempString);
							lua_pushstring(lua_VM, tempString.c_str());
							break;
		}
		}
	}

	//sampgdk::logprintf("PCall: %s - %d", functionName.c_str(), argn - 2);

	//pass how many arguments we passed and calculate how many return value will we have and call the function
	int R = lua_pcall(lua_VM, (argn - 2), LUA_MULTRET, 0);

	//count return
	int nresults = (lua_gettop(lua_VM) - top);

	//stackdump(lua_VM);
	//errstackdump(L);

	if (nresults > 0)
	{
		int stack_end = 1 + nresults;
		ArgReader argR(lua_VM); //return values start at index 3

		for (int stack_index = 1; stack_index < stack_end; stack_index++)
		{
			if (argR.IsBool())
			{
				bool tempBool;
				argR.ReadBool(tempBool);
				//sampgdk::logprintf("B: %d", tempBool);
				lua_pushboolean(L, tempBool);
			}
			else if (argR.IsNumber())
			{
				lua_Number tempNumber;
				argR.ReadLuaNumber(tempNumber);
				//sampgdk::logprintf("N: %d", tempNumber);
				lua_pushnumber(L, tempNumber);
			}
			else if (argR.IsString())
			{
				std::string tempString;
				argR.ReadString(tempString);
				//sampgdk::logprintf("S: %s", tempString.c_str());
				lua_pushstring(L, tempString.c_str());
			}
			else if (argR.IsNil())
			{
				lua_pushnil(L);
			}
			else
			{
				argR.argIndex++;
			}
		}

		//clear the stack, because it can stack to hundreds and thousands of elements
		lua_settop(lua_VM, 0);

		//stackdump(lua_VM);
		//stackdump(L);
	}

	return nresults;
}

LUA_FUNCTION print(lua_State *L)
{
	std::string szFormat;

	ArgReader argReader(L);
	argReader.ReadString(szFormat);

	sampgdk::logprintf(szFormat.c_str());

	return 0;
}

LUA_FUNCTION addEventHandler(lua_State *L)
{
	std::string luaFunction;
	int reference;

	ArgReader argReader(L);
	argReader.ReadString(luaFunction);
	argReader.ReadFunction(reference);
	argReader.ReadFunctionComplete();

	s_Core->RegisterReference(L, luaFunction, reference);
	return 0;
}

LUA_FUNCTION addCommandHandler(lua_State *L)
{
	std::string luaFunction("OPCT_");
	int reference;

	ArgReader argReader(L);
	argReader.ReadString(luaFunction);
	argReader.ReadFunction(reference);
	argReader.ReadFunctionComplete();

	//CUtility::printf("OPCT: %s", luaFunction.c_str());
	s_Core->RegisterReference(L, luaFunction, reference);
	return 0;
}

LUA_FUNCTION setTimer(lua_State *L)
{
	int interval;
	bool repeat;
	int reference;
	CLuaArguments *luaArgs = new CLuaArguments();

	ArgReader argReader(L);
	argReader.ReadNumber(interval);
	argReader.ReadBool(repeat);
	argReader.ReadFunction(reference);
	argReader.ReadArguments(*luaArgs);
	argReader.ReadFunctionComplete();

	int timerid = CTimer::CreateTimer(interval, repeat, L, reference, luaArgs);
	lua_pushnumber(L, timerid);

	return 1;
}

LUA_FUNCTION killTimer(lua_State *L)
{
	int timerid;

	ArgReader argReader(L);
	argReader.ReadNumber(timerid);

	bool success = CTimer::DeleteTimer(timerid, L);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION sendClientMessage(lua_State *L)
{
	int playerID;
	int color;
	std::string message;

	ArgReader argReader(L);
	argReader.ReadNumber(playerID);
	argReader.ReadNumber(color);
	argReader.ReadString(message);

	bool success = SendClientMessage(playerID, color, message.c_str());
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION sendClientMessageToAll(lua_State *L)
{
	int color;
	std::string message;

	ArgReader argReader(L);
	argReader.ReadNumber(color);
	argReader.ReadString(message);

	SendClientMessageToAll(color, message.c_str());
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION sendPlayerMessageToPlayer(lua_State *L)
{
	int playerID;
	int senderID;
	std::string message;

	ArgReader argReader(L);
	argReader.ReadNumber(playerID);
	argReader.ReadNumber(senderID);
	argReader.ReadString(message);

	SendPlayerMessageToPlayer(playerID, senderID, message.c_str());

	return 0;
}

LUA_FUNCTION sendPlayerMessageToAll(lua_State *L)
{
	int senderID;
	std::string message;

	ArgReader argReader(L);
	argReader.ReadNumber(senderID);
	argReader.ReadString(message);

	SendPlayerMessageToAll(senderID, message.c_str());

	return 0;
}

LUA_FUNCTION sendDeathMessage(lua_State *L)
{
	int killer;
	int killee;
	int weapon;

	ArgReader argReader(L);
	argReader.ReadNumber(killer);
	argReader.ReadNumber(killee);
	argReader.ReadNumber(weapon);

	SendDeathMessage(killer, killee, weapon);
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION sendDeathMessageToPlayer(lua_State *L)
{
	int playerID;
	int killer;
	int killee;
	int weapon;

	ArgReader argReader(L);
	argReader.ReadNumber(playerID);
	argReader.ReadNumber(killer);
	argReader.ReadNumber(killee);
	argReader.ReadNumber(weapon);

	bool success = SendDeathMessageToPlayer(playerID, killer, killee, weapon);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION gameTextForAll(lua_State *L)
{
	std::string string;
	int time;
	int style;

	ArgReader argReader(L);
	argReader.ReadString(string);
	argReader.ReadNumber(time);
	argReader.ReadNumber(style);

	GameTextForAll(string.c_str(), time, style);
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION gameTextForPlayer(lua_State *L)
{
	int playerID;
	std::string string;
	int time;
	int style;

	ArgReader argReader(L);
	argReader.ReadNumber(playerID);
	argReader.ReadString(string);
	argReader.ReadNumber(time);
	argReader.ReadNumber(style);

	bool success = GameTextForPlayer(playerID, string.c_str(), time, style);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getTickCount(lua_State *L)
{
	lua_pushnumber(L, GetTickCountEx());

	return 1;
}

LUA_FUNCTION getMaxPlayers(lua_State *L)
{
	lua_pushnumber(L, GetMaxPlayers());

	return 1;
}

LUA_FUNCTION vectorSize(lua_State *L)
{
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);

	lua_pushnumber(L, VectorSize(x, y, z));

	return 1;
}

LUA_FUNCTION getPlayerPoolSize(lua_State *L)
{
	lua_pushnumber(L, GetPlayerPoolSize());

	return 1;
}

LUA_FUNCTION getVehiclePoolSize(lua_State *L)
{
	lua_pushnumber(L, GetVehiclePoolSize());

	return 1;
}

LUA_FUNCTION getActorPoolSize(lua_State *L)
{
	lua_pushnumber(L, GetActorPoolSize());

	return 1;
}

LUA_FUNCTION setGameModeText(lua_State *L)
{
	std::string string;

	ArgReader argReader(L);
	argReader.ReadString(string);

	SetGameModeText(string.c_str());

	return 0;
}

LUA_FUNCTION setTeamCount(lua_State *L)
{
	int count;

	ArgReader argReader(L);
	argReader.ReadNumber(count);

	SetTeamCount(count);

	return 0;
}

LUA_FUNCTION addPlayerClass(lua_State *L)
{
	int modelid;
	float spawn_x;
	float spawn_y;
	float spawn_z;
	float z_angle;
	int weapon1;
	int weapon1_ammo;
	int weapon2;
	int weapon2_ammo;
	int weapon3;
	int weapon3_ammo;

	ArgReader argReader(L);
	argReader.ReadNumber(modelid);
	argReader.ReadFloat(spawn_x);
	argReader.ReadFloat(spawn_y);
	argReader.ReadFloat(spawn_z);
	argReader.ReadFloat(z_angle);
	argReader.ReadNumber(weapon1);
	argReader.ReadNumber(weapon1_ammo);
	argReader.ReadNumber(weapon2);
	argReader.ReadNumber(weapon2_ammo);
	argReader.ReadNumber(weapon3);
	argReader.ReadNumber(weapon3_ammo);

	int classid = AddPlayerClass(modelid, spawn_x, spawn_y, spawn_z, z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
	lua_pushnumber(L, classid);

	return 1;
}

LUA_FUNCTION addPlayerClassEx(lua_State *L)
{
	int teamid;
	int modelid;
	float spawn_x;
	float spawn_y;
	float spawn_z;
	float z_angle;
	int weapon1;
	int weapon1_ammo;
	int weapon2;
	int weapon2_ammo;
	int weapon3;
	int weapon3_ammo;

	ArgReader argReader(L);
	argReader.ReadNumber(teamid);
	argReader.ReadNumber(modelid);
	argReader.ReadFloat(spawn_x);
	argReader.ReadFloat(spawn_y);
	argReader.ReadFloat(spawn_z);
	argReader.ReadFloat(z_angle);
	argReader.ReadNumber(weapon1);
	argReader.ReadNumber(weapon1_ammo);
	argReader.ReadNumber(weapon2);
	argReader.ReadNumber(weapon2_ammo);
	argReader.ReadNumber(weapon3);
	argReader.ReadNumber(weapon3_ammo);

	int classid = AddPlayerClassEx(teamid, modelid, spawn_x, spawn_y, spawn_z, z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
	lua_pushnumber(L, classid);

	return 1;
}

LUA_FUNCTION addStaticVehicle(lua_State *L)
{
	int modelid;
	float spawn_x;
	float spawn_y;
	float spawn_z;
	float z_angle;
	int color1;
	int color2;

	ArgReader argReader(L);
	argReader.ReadNumber(modelid);
	argReader.ReadFloat(spawn_x);
	argReader.ReadFloat(spawn_y);
	argReader.ReadFloat(spawn_z);
	argReader.ReadFloat(z_angle);
	argReader.ReadNumber(color1);
	argReader.ReadNumber(color2);

	int vehicleid = AddStaticVehicle(modelid, spawn_x, spawn_y, spawn_z, z_angle, color1, color2);
	lua_pushnumber(L, vehicleid);

	return 1;
}

LUA_FUNCTION addStaticVehicleEx(lua_State *L)
{
	int modelid;
	float spawn_x;
	float spawn_y;
	float spawn_z;
	float z_angle;
	int color1;
	int color2;
	int respawn_delay;
	bool addsiren = false;

	ArgReader argReader(L);
	argReader.ReadNumber(modelid);
	argReader.ReadFloat(spawn_x);
	argReader.ReadFloat(spawn_y);
	argReader.ReadFloat(spawn_z);
	argReader.ReadFloat(z_angle);
	argReader.ReadNumber(color1);
	argReader.ReadNumber(color2);
	argReader.ReadNumber(respawn_delay);
	argReader.ReadBool(addsiren, false);

	int vehicleid = AddStaticVehicleEx(modelid, spawn_x, spawn_y, spawn_z, z_angle, color1, color2, respawn_delay, addsiren);
	lua_pushnumber(L, vehicleid);

	return 1;
}

LUA_FUNCTION addStaticPickup(lua_State *L)
{
	int model;
	int type;
	float X;
	float Y;
	float Z;
	int virtualworld = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(model);
	argReader.ReadNumber(type);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadNumber(virtualworld, 0);

	int pickup = AddStaticPickup(model, type, X, Y, Z, virtualworld);
	lua_pushnumber(L, pickup);

	return 1;
}

LUA_FUNCTION createPickup(lua_State *L)
{
	int model;
	int type;
	float X;
	float Y;
	float Z;
	int virtualworld = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(model);
	argReader.ReadNumber(type);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadNumber(virtualworld, 0);

	int pickupid = CreatePickup(model, type, X, Y, Z, virtualworld);
	lua_pushnumber(L, pickupid);

	return 1;
}

LUA_FUNCTION destroyPickup(lua_State *L)
{
	int pickup;

	ArgReader argReader(L);
	argReader.ReadNumber(pickup);

	DestroyPickup(pickup);

	return 0;
}

LUA_FUNCTION showNameTags(lua_State *L)
{
	bool show;

	ArgReader argReader(L);
	argReader.ReadBool(show);

	ShowNameTags(show);

	return 0;
}

LUA_FUNCTION showPlayerMarkers(lua_State *L)
{
	int mode;

	ArgReader argReader(L);
	argReader.ReadNumber(mode);

	ShowPlayerMarkers(mode);

	return 0;

}

LUA_FUNCTION gameModeExit(lua_State *L)
{
	GameModeExit();

	return 0;
}

LUA_FUNCTION setWorldTime(lua_State *L)
{
	int hour;

	ArgReader argReader(L);
	argReader.ReadNumber(hour);

	SetWorldTime(hour);

	return 0;
}

LUA_FUNCTION getWeaponName(lua_State *L)
{
	int weaponid;
	char weapon[32];

	ArgReader argReader(L);
	argReader.ReadNumber(weaponid);

	GetWeaponName(weaponid, weapon, sizeof(weapon));
	lua_pushstring(L, weapon);

	return 1;
}

LUA_FUNCTION enableTirePopping(lua_State *L)
{
	bool enable;

	ArgReader argReader(L);
	argReader.ReadBool(enable);

	EnableTirePopping(enable);

	return 0;
}

LUA_FUNCTION enableVehicleFriendlyFire(lua_State *L)
{
	EnableVehicleFriendlyFire();

	return 0;
}

LUA_FUNCTION allowInteriorWeapons(lua_State *L)
{
	bool allow;

	ArgReader argReader(L);
	argReader.ReadBool(allow);

	AllowInteriorWeapons(allow);

	return 0;
}

LUA_FUNCTION setWeather(lua_State *L)
{
	int weatherid;

	ArgReader argReader(L);
	argReader.ReadNumber(weatherid);

	SetWeather(weatherid);

	return 0;
}

LUA_FUNCTION setGravity(lua_State *L)
{
	float gravity;

	ArgReader argReader(L);
	argReader.ReadFloat(gravity);

	SetGravity(gravity);
	lua_pushnumber(L, 1);

	return 1;
}

LUA_FUNCTION allowAdminTeleport(lua_State *L)
{
	bool allow;

	ArgReader argReader(L);
	argReader.ReadBool(allow);

	AllowAdminTeleport(allow);

	return 0;
}

LUA_FUNCTION setDeathDropAmount(lua_State *L)
{
	int amount;

	ArgReader argReader(L);
	argReader.ReadNumber(amount);

	SetDeathDropAmount(amount);

	return 0;
}

LUA_FUNCTION createExplosion(lua_State *L)
{
	float X;
	float Y;
	float Z;
	int type;
	float Radius;

	ArgReader argReader(L);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadNumber(type);
	argReader.ReadFloat(Radius);

	CreateExplosion(X, Y, Z, type, Radius);
	lua_pushnumber(L, 1);

	return 1;
}

LUA_FUNCTION enableZoneNames(lua_State *L)
{
	bool enable;

	ArgReader argReader(L);
	argReader.ReadBool(enable);

	EnableZoneNames(enable);

	return 0;
}

LUA_FUNCTION usePlayerPedAnims(lua_State *L)
{
	UsePlayerPedAnims();

	return 0;
}

LUA_FUNCTION disableInteriorEnterExits(lua_State *L)
{
	DisableInteriorEnterExits();
	lua_pushnumber(L, 1);

	return 1;
}

LUA_FUNCTION setNameTagDrawDistance(lua_State *L)
{
	float distance;

	ArgReader argReader(L);
	argReader.ReadFloat(distance);

	SetNameTagDrawDistance(distance);

	return 0;
}

LUA_FUNCTION disableNameTagLOS(lua_State *L)
{
	DisableNameTagLOS();

	return 0;
}

LUA_FUNCTION limitGlobalChatRadius(lua_State *L)
{
	float chat_radius;

	ArgReader argReader(L);
	argReader.ReadFloat(chat_radius);

	LimitGlobalChatRadius(chat_radius);

	return 0;
}

LUA_FUNCTION limitPlayerMarkerRadius(lua_State *L)
{
	float marker_radius;

	ArgReader argReader(L);
	argReader.ReadFloat(marker_radius);

	LimitPlayerMarkerRadius(marker_radius);

	return 0;
}

LUA_FUNCTION connectNPC(lua_State *L)
{
	std::string name;
	std::string script;

	ArgReader argReader(L);
	argReader.ReadString(name);
	argReader.ReadString(script);

	ConnectNPC(name.c_str(), script.c_str());

	return 0;
}

LUA_FUNCTION isPlayerNPC(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	bool isNPC = IsPlayerNPC(playerid);
	lua_pushboolean(L, isNPC);

	return 1;
}

LUA_FUNCTION isPlayerAdmin(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	bool isAdmin = IsPlayerAdmin(playerid);
	lua_pushboolean(L, isAdmin);

	return 1;
}

LUA_FUNCTION kickPlayer(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	Kick(playerid);
	lua_pushnumber(L, 1);

	return 1;
}

LUA_FUNCTION banPlayer(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	Ban(playerid);

	return 0;
}

LUA_FUNCTION banPlayerEx(lua_State *L)
{
	int playerid;
	std::string reason;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(reason);

	BanEx(playerid, reason.c_str());

	return 0;
}

LUA_FUNCTION sendRconCommand(lua_State *L)
{
	std::string command;

	ArgReader argReader(L);
	argReader.ReadString(command);

	SendRconCommand(command.c_str());
	lua_pushnumber(L, 1);

	return 1;
}

LUA_FUNCTION getServerVarAsString(lua_State *L)
{
	std::string varname;
	char buffer[128];

	ArgReader argReader(L);
	argReader.ReadString(varname);

	GetServerVarAsString(varname.c_str(), buffer, sizeof(buffer));
	lua_pushstring(L, buffer);

	return 1;
}

LUA_FUNCTION getServerVarAsInt(lua_State *L)
{
	std::string varname;

	ArgReader argReader(L);
	argReader.ReadString(varname);

	int varInt = GetServerVarAsInt(varname.c_str());
	lua_pushnumber(L, varInt);

	return 1;
}

LUA_FUNCTION getServerVarAsBool(lua_State *L)
{
	std::string varname;

	ArgReader argReader(L);
	argReader.ReadString(varname);

	bool varBool = GetServerVarAsBool(varname.c_str());
	lua_pushboolean(L, varBool);

	return 1;
}

LUA_FUNCTION getPlayerNetworkStats(lua_State *L)
{
	int playerid;
	char retstr[512];

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerNetworkStats(playerid, retstr, sizeof(retstr));
	lua_pushstring(L, retstr);

	return 1;
}

LUA_FUNCTION getNetworkStats(lua_State *L)
{
	char retstr[512];
	
	GetNetworkStats(retstr, sizeof(retstr));
	lua_pushstring(L, retstr);

	return 1;
}

LUA_FUNCTION getPlayerVersion(lua_State *L)
{
	int playerid;
	char version[40];

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerVersion(playerid, version, sizeof(version));
	lua_pushstring(L, version);

	return 1;
}

LUA_FUNCTION blockIpAddress(lua_State *L)
{
	std::string ip_address;
	int timems;

	ArgReader argReader(L);
	argReader.ReadString(ip_address);
	argReader.ReadNumber(timems);

	BlockIpAddress(ip_address.c_str(), timems);

	return 0;
}

LUA_FUNCTION unBlockIpAddress(lua_State *L)
{
	std::string ip_address;

	ArgReader argReader(L);
	argReader.ReadString(ip_address);

	UnBlockIpAddress(ip_address.c_str());

	return 0;
}

LUA_FUNCTION getServerTickRate(lua_State *L)
{
	int tickRate = GetServerTickRate();
	lua_pushnumber(L, tickRate);

	return 1;
}

LUA_FUNCTION netStats_GetConnectedTime(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int connectedTime = NetStats_GetConnectedTime(playerid);
	lua_pushnumber(L, connectedTime);

	return 1;
}

LUA_FUNCTION netStats_MessagesReceived(lua_State *L)
{
	int playerid;
	
	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int messagesReceived = NetStats_MessagesReceived(playerid);
	lua_pushnumber(L, messagesReceived);

	return 1;
}

LUA_FUNCTION netStats_BytesReceived(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int bytesReceived = NetStats_BytesReceived(playerid);
	lua_pushnumber(L, bytesReceived);

	return 1;
}

LUA_FUNCTION netStats_MessagesSent(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int messagesSent = NetStats_MessagesSent(playerid);
	lua_pushnumber(L, messagesSent);

	return 1;
}

LUA_FUNCTION netStats_BytesSent(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int bytesSent = NetStats_BytesSent(playerid);
	lua_pushnumber(L, bytesSent);

	return 1;
}

LUA_FUNCTION netStats_MessagesRecvPerSecond(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int messagesPerSecond = NetStats_MessagesRecvPerSecond(playerid);
	lua_pushnumber(L, messagesPerSecond);

	return 1;
}

LUA_FUNCTION netStats_PacketLossPercent(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	float packetLoss = NetStats_PacketLossPercent(playerid);
	lua_pushnumber(L, packetLoss);

	return 1;
}

LUA_FUNCTION netStats_ConnectionStatus(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int conStatus = NetStats_ConnectionStatus(playerid);
	lua_pushnumber(L, conStatus);

	return 1;
}

LUA_FUNCTION netStats_GetIpPort(lua_State *L)
{
	int playerid;
	char ip_port[24];

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	NetStats_GetIpPort(playerid, ip_port, sizeof(ip_port));
	lua_pushstring(L, ip_port);

	return 1;
}

LUA_FUNCTION createMenu(lua_State *L)
{
	std::string title;
	int columns;
	float x;
	float y;
	float col1width;
	float col2width;

	ArgReader argReader(L);
	argReader.ReadString(title);
	argReader.ReadNumber(columns);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(col1width);
	argReader.ReadFloat(col2width);

	int menuid = CreateMenuEx(title.c_str(), columns, x, y, col1width, col2width);
	lua_pushnumber(L, menuid);

	return 1;
}

LUA_FUNCTION destroyMenu(lua_State *L)
{
	int menuid;

	ArgReader argReader(L);
	argReader.ReadNumber(menuid);

	bool success = DestroyMenuEx(menuid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION addMenuItem(lua_State *L)
{
	int menuid;
	int column;
	std::string menutext;

	ArgReader argReader(L);
	argReader.ReadNumber(menuid);
	argReader.ReadNumber(column);
	argReader.ReadString(menutext);

	int rowIndex = AddMenuItem(menuid, column, menutext.c_str());
	lua_pushnumber(L, rowIndex);

	return 1;
}

LUA_FUNCTION setMenuColumnHeader(lua_State *L)
{
	int menuid;
	int column;
	std::string columnheader;

	ArgReader argReader(L);
	argReader.ReadNumber(menuid);
	argReader.ReadNumber(column);
	argReader.ReadString(columnheader);

	SetMenuColumnHeader(menuid, column, columnheader.c_str());

	return 0;
}

LUA_FUNCTION showMenuForPlayer(lua_State *L)
{
	int menuid;
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(menuid);
	argReader.ReadNumber(playerid);

	bool success = ShowMenuForPlayer(menuid, playerid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION hideMenuForPlayer(lua_State *L)
{
	int menuid;
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(menuid);
	argReader.ReadNumber(playerid);

	bool success = HideMenuForPlayer(menuid, playerid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION isValidMenu(lua_State *L)
{
	int menuid;

	ArgReader argReader(L);
	argReader.ReadNumber(menuid);

	bool isValid = IsValidMenu(menuid);
	lua_pushboolean(L, isValid);

	return 1;
}

LUA_FUNCTION disableMenu(lua_State *L)
{
	int menuid;

	ArgReader argReader(L);
	argReader.ReadNumber(menuid);

	DisableMenu(menuid);

	return 0;
}

LUA_FUNCTION disableMenuRow(lua_State *L)
{
	int menuid;
	int row;

	ArgReader argReader(L);
	argReader.ReadNumber(menuid);
	argReader.ReadNumber(row);

	bool isTrue = DisableMenuRow(menuid, row);
	lua_pushboolean(L, isTrue);

	return 1;
}

LUA_FUNCTION getPlayerMenu(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int menuid = GetPlayerMenu(playerid);
	lua_pushnumber(L, menuid);

	return 1;
}

LUA_FUNCTION textDrawCreate(lua_State *L)
{
	float x;
	float y;
	std::string text;

	ArgReader argReader(L);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadString(text);

	int textDrawID = TextDrawCreate(x, y, text.c_str());
	lua_pushnumber(L, textDrawID);

	return 1;
}

LUA_FUNCTION textDrawDestroy(lua_State *L)
{
	int text;

	ArgReader argReader(L);
	argReader.ReadNumber(text);

	TextDrawDestroy(text);

	return 0;
}

LUA_FUNCTION textDrawLetterSize(lua_State *L)
{
	int text;
	float x;
	float y;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);

	TextDrawLetterSize(text, x, y);

	return 0;
}

LUA_FUNCTION textDrawTextSize(lua_State *L)
{
	int text;
	float x;
	float y;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);

	TextDrawTextSize(text, x, y);

	return 0;
}

LUA_FUNCTION textDrawAlignment(lua_State *L)
{
	int text;
	int alignment;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadNumber(alignment);

	TextDrawAlignment(text, alignment);

	return 0;
}

LUA_FUNCTION textDrawColor(lua_State *L)
{
	int text;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadNumber(color);

	TextDrawColor(text, color);

	return 0;
}

LUA_FUNCTION textDrawUseBox(lua_State *L)
{
	int text;
	bool use;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadBool(use);

	bool success = TextDrawUseBox(text, use);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION textDrawBoxColor(lua_State *L)
{
	int text;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadNumber(color);

	TextDrawBoxColor(text, color);

	return 0;
}

LUA_FUNCTION textDrawSetShadow(lua_State *L)
{
	int text;
	int size;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadNumber(size);

	bool success = TextDrawSetShadow(text, size);
	lua_pushboolean(L, success);
	
	return 1;
}

LUA_FUNCTION textDrawSetOutline(lua_State *L)
{
	int text;
	int size;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadNumber(size);

	TextDrawSetOutline(text, size);

	return 0;
}

LUA_FUNCTION textDrawBackgroundColor(lua_State *L)
{
	int text;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadNumber(color);

	TextDrawBackgroundColor(text, color);

	return 0;
}

LUA_FUNCTION textDrawFont(lua_State *L)
{
	int text;
	int font;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadNumber(font);

	TextDrawFont(text, font);

	return 0;
}

LUA_FUNCTION textDrawSetProportional(lua_State *L)
{
	int text;
	bool set;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadBool(set);

	TextDrawSetProportional(text, set);

	return 0;
}

LUA_FUNCTION textDrawSetSelectable(lua_State *L)
{
	int text;
	bool set;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadBool(set);

	TextDrawSetSelectable(text, set);

	return 0;
}

LUA_FUNCTION textDrawShowForPlayer(lua_State *L)
{
	int playerid;
	int text;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);

	bool success = TextDrawShowForPlayer(playerid, text);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION textDrawHideForPlayer(lua_State *L)
{
	int playerid;
	int text;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);

	TextDrawHideForPlayer(playerid, text);

	return 0;
}

LUA_FUNCTION textDrawShowForAll(lua_State *L)
{
	int text;

	ArgReader argReader(L);
	argReader.ReadNumber(text);

	bool success = TextDrawShowForAll(text);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION textDrawHideForAll(lua_State *L)
{
	int text;

	ArgReader argReader(L);
	argReader.ReadNumber(text);

	TextDrawHideForAll(text);

	return 0;
}

LUA_FUNCTION textDrawSetString(lua_State *L)
{
	int text;
	std::string string;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadString(string);

	TextDrawSetString(text, string.c_str());

	return 0;
}

LUA_FUNCTION textDrawSetPreviewModel(lua_State *L)
{
	int text;
	int modelindex;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadNumber(modelindex);

	TextDrawSetPreviewModel(text, modelindex);

	return 0;
}

LUA_FUNCTION textDrawSetPreviewRot(lua_State *L)
{
	int text;
	float fRotX;
	float fRotY;
	float fRotZ;
	float fZoom = 1.0;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadFloat(fRotX);
	argReader.ReadFloat(fRotY);
	argReader.ReadFloat(fRotZ);
	argReader.ReadFloat(fZoom, 1.0);

	TextDrawSetPreviewRot(text, fRotX, fRotY, fRotZ, fZoom);

	return 0;
}

LUA_FUNCTION textDrawSetPreviewVehCol(lua_State *L)
{
	int text;
	int color1;
	int color2;

	ArgReader argReader(L);
	argReader.ReadNumber(text);
	argReader.ReadNumber(color1);
	argReader.ReadNumber(color2);

	TextDrawSetPreviewVehCol(text, color1, color2);

	return 0;
}

LUA_FUNCTION gangZoneCreate(lua_State *L)
{
	float minx;
	float miny;
	float maxx;
	float maxy;

	ArgReader argReader(L);
	argReader.ReadFloat(minx);
	argReader.ReadFloat(miny);
	argReader.ReadFloat(maxx);
	argReader.ReadFloat(maxy);

	int gangID = GangZoneCreate(minx, miny, maxx, maxy);
	lua_pushnumber(L, gangID);

	return 1;
}

LUA_FUNCTION gangZoneDestroy(lua_State *L)
{
	int zone;

	ArgReader argReader(L);
	argReader.ReadNumber(zone);

	GangZoneDestroy(zone);

	return 0;
}

LUA_FUNCTION gangZoneShowForPlayer(lua_State *L)
{
	int playerid;
	int zone;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(zone);
	argReader.ReadNumber(color);

	bool success = GangZoneShowForPlayer(playerid, zone, color);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION gangZoneShowForAll(lua_State *L)
{
	int zone;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(zone);
	argReader.ReadNumber(color);

	bool success = GangZoneShowForAll(zone, color);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION gangZoneHideForPlayer(lua_State *L)
{
	int playerid;
	int zone;
	
	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(zone);

	GangZoneHideForPlayer(playerid, zone);

	return 0;
}

LUA_FUNCTION gangZoneHideForAll(lua_State *L)
{
	int zone;

	ArgReader argReader(L);
	argReader.ReadNumber(zone);

	GangZoneHideForAll(zone);

	return 0;
}

LUA_FUNCTION gangZoneFlashForPlayer(lua_State *L)
{
	int playerid;
	int zone;
	int flashcolor;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(zone);
	argReader.ReadNumber(flashcolor);

	GangZoneFlashForPlayer(playerid, zone, flashcolor);

	return 0;
}

LUA_FUNCTION gangZoneFlashForAll(lua_State *L)
{
	int zone;
	int flashcolor;

	ArgReader argReader(L);
	argReader.ReadNumber(zone);
	argReader.ReadNumber(flashcolor);

	GangZoneFlashForAll(zone, flashcolor);

	return 0;
}

LUA_FUNCTION gangZoneStopFlashForPlayer(lua_State *L)
{
	int playerid;
	int zone;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(zone);

	GangZoneStopFlashForPlayer(playerid, zone);

	return 0;
}

LUA_FUNCTION gangZoneStopFlashForAll(lua_State *L)
{
	int zone;

	ArgReader argReader(L);
	argReader.ReadNumber(zone);

	bool success = GangZoneStopFlashForAll(zone);
	lua_pushboolean(L, success);
	
	return 0;
}

LUA_FUNCTION create3DTextLabel(lua_State *L)
{
	std::string text;
	int color;
	float X;
	float Y;
	float Z;
	float DrawDistance;
	int virtualworld;
	bool testLOS = false;

	ArgReader argReader(L);
	argReader.ReadString(text);
	argReader.ReadNumber(color);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadFloat(DrawDistance);
	argReader.ReadNumber(virtualworld);
	argReader.ReadBool(testLOS, false);

	int textLabel = Create3DTextLabel(text.c_str(), color, X, Y, Z, DrawDistance, virtualworld, testLOS);
	lua_pushnumber(L, textLabel);

	return 1;
}

LUA_FUNCTION delete3DTextLabel(lua_State *L)
{
	int id;

	ArgReader argReader(L);
	argReader.ReadNumber(id);

	bool success = Delete3DTextLabel(id);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION attach3DTextLabelToPlayer(lua_State *L)
{
	int id;
	int playerid;
	float OffsetX;
	float OffsetY;
	float OffsetZ;

	ArgReader argReader(L);
	argReader.ReadNumber(id);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(OffsetX);
	argReader.ReadFloat(OffsetY);
	argReader.ReadFloat(OffsetZ);

	bool success = Attach3DTextLabelToPlayer(id, playerid, OffsetX, OffsetY, OffsetZ);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION attach3DTextLabelToVehicle(lua_State *L)
{
	int id;
	int vehicleid;
	float OffsetX;
	float OffsetY;
	float OffsetZ;

	ArgReader argReader(L);
	argReader.ReadNumber(id);
	argReader.ReadNumber(vehicleid);
	argReader.ReadFloat(OffsetX);
	argReader.ReadFloat(OffsetY);
	argReader.ReadFloat(OffsetZ);

	Attach3DTextLabelToVehicle(id, vehicleid, OffsetX, OffsetY, OffsetZ);

	return 0;
}

LUA_FUNCTION update3DTextLabelText(lua_State *L)
{
	int id;
	int color;
	std::string text;

	ArgReader argReader(L);
	argReader.ReadNumber(id);
	argReader.ReadNumber(color);
	argReader.ReadString(text);

	Update3DTextLabelText(id, color, text.c_str());

	return 0;
}

LUA_FUNCTION createPlayer3DTextLabel(lua_State *L)
{
	int playerid;
	std::string text;
	int color;
	float X;
	float Y;
	float Z;
	float DrawDistance;
	int attachedplayer = INVALID_PLAYER_ID;
	int attachedvehicle = INVALID_VEHICLE_ID;
	bool testLOS = false;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(text);
	argReader.ReadNumber(color);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadFloat(DrawDistance);
	argReader.ReadNumber(attachedplayer, INVALID_PLAYER_ID);
	argReader.ReadNumber(attachedvehicle, INVALID_VEHICLE_ID);
	argReader.ReadBool(testLOS, false);

	int playerTextLabel = CreatePlayer3DTextLabel(playerid, text.c_str(), color, X, Y, Z, DrawDistance, attachedplayer, attachedvehicle, testLOS);
	lua_pushnumber(L, playerTextLabel);

	return 1;
}

LUA_FUNCTION deletePlayer3DTextLabel(lua_State *L)
{
	int playerid;
	int id;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(id);

	bool success = DeletePlayer3DTextLabel(playerid, id);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION updatePlayer3DTextLabelText(lua_State *L)
{
	int playerid;
	int id;
	int color;
	std::string text;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(id);
	argReader.ReadNumber(color);
	argReader.ReadString(text);

	UpdatePlayer3DTextLabelText(playerid, id, color, text.c_str());

	return 0;
}

LUA_FUNCTION showPlayerDialog(lua_State *L)
{
	int playerid;
	int dialogid;
	int style;
	std::string caption;
	std::string info;
	std::string button1;
	std::string button2;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(dialogid);
	argReader.ReadNumber(style);
	argReader.ReadString(caption);
	argReader.ReadString(info);
	argReader.ReadString(button1);
	argReader.ReadString(button2);

	ShowPlayerDialog(playerid, dialogid, style, caption.c_str(), info.c_str(), button1.c_str(), button2.c_str());

	return 0;
}

LUA_FUNCTION lua_HTTP(lua_State *L)
{
	int index;
	int type;
	std::string url;
	std::string data;
	//std::string callback;

	ArgReader argReader(L);
	argReader.ReadNumber(index);
	argReader.ReadNumber(type);
	argReader.ReadString(url);
	argReader.ReadString(data);
	//argReader.ReadString(callback);

	bool success = HTTP(index, type, url.c_str(), data.c_str());
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION createActor(lua_State *L)
{
	int modelid;
	float X;
	float Y;
	float Z;
	float Rotation;

	ArgReader argReader(L);
	argReader.ReadNumber(modelid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadFloat(Rotation);

	int actorid = CreateActor(modelid, X, Y, Z, Rotation);
	lua_pushnumber(L, actorid);

	return 1;
}

LUA_FUNCTION destroyActor(lua_State *L)
{
	int actorid;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);

	bool success = DestroyActor(actorid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION isActorStreamedIn(lua_State *L)
{
	int actorid;
	int forplayerid;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);
	argReader.ReadNumber(forplayerid);

	bool isStreamedIn = IsActorStreamedIn(actorid, forplayerid);
	lua_pushboolean(L, isStreamedIn);

	return 1;
}

LUA_FUNCTION setActorVirtualWorld(lua_State *L)
{
	int actorid;
	int vworld;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);
	argReader.ReadNumber(vworld);

	bool success = SetActorVirtualWorld(actorid, vworld);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getActorVirtualWorld(lua_State *L)
{
	int actorid;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);

	int vworld = GetActorVirtualWorld(actorid);
	lua_pushnumber(L, vworld);

	return 1;
}

LUA_FUNCTION applyActorAnimation(lua_State *L)
{
	int actorid;
	std::string animlib;
	std::string animname;
	float fDelta;
	bool loop;
	bool lockx;
	bool locky;
	bool freeze;
	int time;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);
	argReader.ReadString(animlib);
	argReader.ReadString(animname);
	argReader.ReadFloat(fDelta);
	argReader.ReadBool(loop);
	argReader.ReadBool(lockx);
	argReader.ReadBool(locky);
	argReader.ReadBool(freeze);
	argReader.ReadNumber(time);

	bool success = ApplyActorAnimation(actorid, animlib.c_str(), animname.c_str(), fDelta, loop, lockx, locky, freeze, time);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION clearActorAnimations(lua_State *L)
{
	int actorid;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);

	bool success = ClearActorAnimations(actorid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION setActorPos(lua_State *L)
{
	int actorid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);

	bool success = SetActorPos(actorid, X, Y, Z);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getActorPos(lua_State *L)
{
	int actorid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);

	GetActorPos(actorid, &X, &Y, &Z);
	lua_pushnumber(L, X);
	lua_pushnumber(L, Y);
	lua_pushnumber(L, Z);

	return 3;
}

LUA_FUNCTION setActorFacingAngle(lua_State *L)
{
	int actorid;
	float ang;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);
	argReader.ReadFloat(ang);

	bool success = SetActorFacingAngle(actorid, ang);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getActorFacingAngle(lua_State *L)
{
	int actorid;
	float ang;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);

	GetActorFacingAngle(actorid, &ang);
	lua_pushnumber(L, ang);

	return 1;
}

LUA_FUNCTION setActorHealth(lua_State *L)
{
	int actorid;
	float health;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);
	argReader.ReadFloat(health);

	bool success = SetActorHealth(actorid, health);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getActorHealth(lua_State *L)
{
	int actorid;
	float health;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);

	GetActorHealth(actorid, &health);
	lua_pushnumber(L, health);

	return 1;
}

LUA_FUNCTION setActorInvulnerable(lua_State *L)
{
	int actorid;
	bool invulnerable = true;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);
	argReader.ReadBool(invulnerable, true);

	bool success = SetActorInvulnerable(actorid, invulnerable);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION isActorInvulnerable(lua_State *L)
{
	int actorid;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);

	bool isInvulnerable = IsActorInvulnerable(actorid);
	lua_pushboolean(L, isInvulnerable);

	return 1;
}

LUA_FUNCTION isValidActor(lua_State *L)
{
	int actorid;

	ArgReader argReader(L);
	argReader.ReadNumber(actorid);

	bool isActor = IsValidActor(actorid);
	lua_pushboolean(L, isActor);

	return 1;
}

LUA_FUNCTION createObject(lua_State *L)
{
	int modelid;
	float X;
	float Y;
	float Z;
	float rX;
	float rY;
	float rZ;
	float DrawDistance = 0.0;

	ArgReader argReader(L);
	argReader.ReadNumber(modelid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadFloat(rX);
	argReader.ReadFloat(rY);
	argReader.ReadFloat(rZ);
	argReader.ReadFloat(DrawDistance, 0.0);

	int objectid = CreateObject(modelid, X, Y, Z, rX, rY, rZ, DrawDistance);
	lua_pushnumber(L, objectid);

	return 1;
}

LUA_FUNCTION attachObjectToVehicle(lua_State *L)
{
	int objectid;
	int vehicleid;
	float OffsetX;
	float OffsetY;
	float OffsetZ;
	float RotX;
	float RotY;
	float RotZ;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);
	argReader.ReadNumber(vehicleid);
	argReader.ReadFloat(OffsetX);
	argReader.ReadFloat(OffsetY);
	argReader.ReadFloat(OffsetZ);
	argReader.ReadFloat(RotX);
	argReader.ReadFloat(RotY);
	argReader.ReadFloat(RotZ);

	AttachObjectToVehicle(objectid, vehicleid, OffsetX, OffsetY, OffsetZ, RotX, RotY, RotZ);

	return 0;
}

LUA_FUNCTION attachObjectToObject(lua_State *L)
{
	int objectid;
	int attachtoid;
	float OffsetX;
	float OffsetY;
	float OffsetZ;
	float RotX;
	float RotY;
	float RotZ;
	bool SyncRotation = true;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);
	argReader.ReadNumber(attachtoid);
	argReader.ReadFloat(OffsetX);
	argReader.ReadFloat(OffsetY);
	argReader.ReadFloat(OffsetZ);
	argReader.ReadFloat(RotX);
	argReader.ReadFloat(RotY);
	argReader.ReadFloat(RotZ);
	argReader.ReadBool(SyncRotation, true);

	bool success = AttachObjectToObject(objectid, attachtoid, OffsetX, OffsetY, OffsetZ, RotX, RotY, RotZ, SyncRotation);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION attachObjectToPlayer(lua_State *L)
{
	int objectid;
	int playerid;
	float OffsetX;
	float OffsetY;
	float OffsetZ;
	float RotX;
	float RotY;
	float RotZ;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(OffsetX);
	argReader.ReadFloat(OffsetY);
	argReader.ReadFloat(OffsetZ);
	argReader.ReadFloat(RotX);
	argReader.ReadFloat(RotY);
	argReader.ReadFloat(RotZ);

	AttachObjectToPlayer(objectid, playerid, OffsetX, OffsetY, OffsetZ, RotX, RotY, RotZ);
	lua_pushboolean(L, false);

	return 1;
}

LUA_FUNCTION setObjectPos(lua_State *L)
{
	int objectid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);

	SetObjectPos(objectid, X, Y, Z);
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION getObjectPos(lua_State *L)
{
	int objectid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);

	GetObjectPos(objectid, &X, &Y, &Z);
	lua_pushnumber(L, X);
	lua_pushnumber(L, Y);
	lua_pushnumber(L, Z);

	return 3;
}

LUA_FUNCTION setObjectRot(lua_State *L)
{
	int objectid;
	float RotX;
	float RotY;
	float RotZ;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);
	argReader.ReadFloat(RotX);
	argReader.ReadFloat(RotY);
	argReader.ReadFloat(RotZ);

	SetObjectRot(objectid, RotX, RotY, RotZ);
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION getObjectRot(lua_State *L)
{
	int objectid;
	float RotX;
	float RotY;
	float RotZ;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);

	GetObjectRot(objectid, &RotX, &RotY, &RotZ);
	lua_pushnumber(L, RotX);
	lua_pushnumber(L, RotY);
	lua_pushnumber(L, RotZ);

	return 3;
}

LUA_FUNCTION getObjectModel(lua_State *L)
{
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);

	int modelid = GetObjectModel(objectid);
	lua_pushnumber(L, modelid);

	return 1;
}

LUA_FUNCTION setObjectNoCameraCol(lua_State *L)
{
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);

	bool success = SetObjectNoCameraCol(objectid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION isValidObject(lua_State *L)
{
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);

	bool isObject = IsValidObject(objectid);
	lua_pushboolean(L, isObject);

	return 1;
}

LUA_FUNCTION destroyObject(lua_State *L)
{
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);

	DestroyObject(objectid);

	return 0;
}

LUA_FUNCTION moveObject(lua_State *L)
{
	int objectid;
	float X;
	float Y;
	float Z;
	float Speed;
	float RotX = -1000.0;
	float RotY = -1000.0;
	float RotZ = -1000.0;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadFloat(Speed);
	argReader.ReadFloat(RotX, -1000.0);
	argReader.ReadFloat(RotY, -1000.0);
	argReader.ReadFloat(RotZ, -1000.0);

	int moveTime = MoveObject(objectid, X, Y, Z, Speed, RotX, RotY, RotZ);
	lua_pushnumber(L, moveTime);

	return 1;
}

LUA_FUNCTION stopObject(lua_State *L)
{
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);

	StopObject(objectid);

	return 0;
}

LUA_FUNCTION isObjectMoving(lua_State *L)
{
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);

	bool isOMoving = IsObjectMoving(objectid);
	lua_pushboolean(L, isOMoving);

	return 1;
}

LUA_FUNCTION editObject(lua_State *L)
{
	int playerid;
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);

	bool success = EditObject(playerid, objectid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION editPlayerObject(lua_State *L)
{
	int playerid;
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);

	bool success = EditPlayerObject(playerid, objectid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION selectObject(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	SelectObjectEx(playerid);

	return 0;
}

LUA_FUNCTION cancelEdit(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	CancelEdit(playerid);

	return 0;
}

LUA_FUNCTION createPlayerObject(lua_State *L)
{
	int playerid;
	int modelid;
	float X;
	float Y;
	float Z;
	float rX;
	float rY;
	float rZ;
	float DrawDistance = 0.0;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(modelid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadFloat(rX);
	argReader.ReadFloat(rY);
	argReader.ReadFloat(rZ);
	argReader.ReadFloat(DrawDistance, 0.0);

	int playerobjectid = CreatePlayerObject(playerid, modelid, X, Y, Z, rX, rY, rZ, DrawDistance);
	lua_pushnumber(L, playerobjectid);

	return 1;
}

LUA_FUNCTION attachPlayerObjectToVehicle(lua_State *L)
{
	int playerid;
	int objectid;
	int vehicleid;
	float fOffsetX;
	float fOffsetY;
	float fOffsetZ;
	float fRotX;
	float fRotY;
	float fRotZ;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);
	argReader.ReadNumber(vehicleid);
	argReader.ReadFloat(fOffsetX);
	argReader.ReadFloat(fOffsetY);
	argReader.ReadFloat(fOffsetZ);
	argReader.ReadFloat(fRotX);
	argReader.ReadFloat(fRotY);
	argReader.ReadFloat(fRotZ);

	AttachPlayerObjectToVehicle(playerid, objectid, vehicleid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ);

	return 0;
}

LUA_FUNCTION setPlayerObjectPos(lua_State *L)
{
	int playerid;
	int objectid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);

	bool success = SetPlayerObjectPos(playerid, objectid, X, Y, Z);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerObjectPos(lua_State *L)
{
	int playerid;
	int objectid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);
	
	GetPlayerObjectPos(playerid, objectid, &X, &Y, &Z);
	lua_pushnumber(L, X);
	lua_pushnumber(L, Y);
	lua_pushnumber(L, Z);

	return 3;
}

LUA_FUNCTION setPlayerObjectRot(lua_State *L)
{
	int playerid;
	int objectid;
	float RotX;
	float RotY;
	float RotZ;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);
	argReader.ReadFloat(RotX);
	argReader.ReadFloat(RotY);
	argReader.ReadFloat(RotZ);

	bool success = SetPlayerObjectPos(playerid, objectid, RotX, RotY, RotZ);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerObjectRot(lua_State *L)
{
	int playerid;
	int objectid;
	float RotX;
	float RotY;
	float RotZ;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);
	
	GetPlayerObjectRot(playerid, objectid, &RotX, &RotY, &RotZ);
	lua_pushnumber(L, RotX);
	lua_pushnumber(L, RotY);
	lua_pushnumber(L, RotZ);

	return 3;
}

LUA_FUNCTION getPlayerObjectModel(lua_State *L)
{
	int playerid;
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);

	int playerobjectmodel = GetPlayerObjectModel(playerid, objectid);
	lua_pushnumber(L, playerobjectmodel);

	return 1;
}

LUA_FUNCTION setPlayerObjectNoCameraCol(lua_State *L)
{
	int playerid;
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);

	SetPlayerObjectNoCameraCol(playerid, objectid);
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION isValidPlayerObject(lua_State *L)
{
	int playerid;
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);

	bool isPlayerObject = IsValidPlayerObject(playerid, objectid);
	lua_pushboolean(L, isPlayerObject);

	return 1;
}

LUA_FUNCTION destroyPlayerObject(lua_State *L)
{
	int playerid;
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);

	DestroyPlayerObject(playerid, objectid);

	return 0;
}

LUA_FUNCTION movePlayerObject(lua_State *L)
{
	int playerid;
	int objectid;
	float X;
	float Y;
	float Z;
	float Speed;
	float RotX = -1000.0;
	float RotY = -1000.0;
	float RotZ = -1000.0;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadFloat(Speed);
	argReader.ReadFloat(RotX, -1000.0);
	argReader.ReadFloat(RotY, -1000.0);
	argReader.ReadFloat(RotZ, -1000.0);

	int moveTime = MovePlayerObject(playerid, objectid, X, Y, Z, Speed, RotX, RotY, RotZ);
	lua_pushnumber(L, moveTime);

	return 1;
}

LUA_FUNCTION stopPlayerObject(lua_State *L)
{
	int playerid;
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);

	StopPlayerObject(playerid, objectid);

	return 0;
}

LUA_FUNCTION isPlayerObjectMoving(lua_State *L)
{
	int playerid;
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);

	bool isPOMoving = IsPlayerObjectMoving(playerid, objectid);
	lua_pushboolean(L, isPOMoving);

	return 1;
}

LUA_FUNCTION attachPlayerObjectToPlayer(lua_State *L)
{
	int objectplayer;
	int objectid;
	int attachplayer;
	float OffsetX;
	float OffsetY;
	float OffsetZ;
	float rX;
	float rY;
	float rZ;

	ArgReader argReader(L);
	argReader.ReadNumber(objectplayer);
	argReader.ReadNumber(objectid);
	argReader.ReadNumber(attachplayer);
	argReader.ReadFloat(OffsetX);
	argReader.ReadFloat(OffsetY);
	argReader.ReadFloat(OffsetZ);
	argReader.ReadFloat(rX);
	argReader.ReadFloat(rY);
	argReader.ReadFloat(rZ);

	AttachPlayerObjectToPlayer(objectplayer, objectid, attachplayer, OffsetX, OffsetY, OffsetZ, rX, rY, rZ);

	return 0;
}

LUA_FUNCTION setObjectMaterial(lua_State *L)
{
	int objectid;
	int materialindex;
	int modelid;
	std::string txdname;
	std::string texturename;
	int materialcolor = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);
	argReader.ReadNumber(materialindex);
	argReader.ReadNumber(modelid);
	argReader.ReadString(txdname);
	argReader.ReadString(texturename);
	argReader.ReadNumber(materialcolor, 0);

	SetObjectMaterial(objectid, materialindex, modelid, txdname.c_str(), texturename.c_str(), materialcolor);

	return 0;
}

LUA_FUNCTION setPlayerObjectMaterial(lua_State *L)
{
	int playerid;
	int objectid;
	int materialindex;
	int modelid;
	std::string txdname;
	std::string texturename;
	int materialcolor = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);
	argReader.ReadNumber(materialindex);
	argReader.ReadNumber(modelid);
	argReader.ReadString(txdname);
	argReader.ReadString(texturename);
	argReader.ReadNumber(materialcolor, 0);

	SetPlayerObjectMaterial(playerid, objectid, materialindex, modelid, txdname.c_str(), texturename.c_str(), materialcolor);

	return 0;
}

LUA_FUNCTION setObjectMaterialText(lua_State *L)
{
	int objectid;
	std::string text;
	int materialindex = 0;
	int materialsize = OBJECT_MATERIAL_SIZE_256x128;
	std::string fontface;
	int fontsize = 24;
	bool bold = 1;
	int fontcolor = 0xFFFFFFFF;
	int backcolor = 0;
	int textalignment = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(objectid);
	argReader.ReadString(text);
	argReader.ReadNumber(materialindex);
	argReader.ReadNumber(materialsize, OBJECT_MATERIAL_SIZE_256x128);
	argReader.ReadString(fontface, "Arial");
	argReader.ReadNumber(fontsize, 24);
	argReader.ReadBool(bold, true);
	argReader.ReadNumber(fontcolor, 0xFFFFFFFF);
	argReader.ReadNumber(backcolor, 0);
	argReader.ReadNumber(textalignment, 0);

	bool success = SetObjectMaterialText(objectid, text.c_str(), materialindex, materialsize, fontface.c_str(), fontsize, bold, fontcolor, backcolor, textalignment);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION setPlayerObjectMaterialText(lua_State *L)
{
	int playerid;
	int objectid;
	std::string text;
	int materialindex = 0;
	int materialsize = OBJECT_MATERIAL_SIZE_256x128;
	std::string fontface;
	int fontsize = 24;
	bool bold = 1;
	int fontcolor = 0xFFFFFFFF;
	int backcolor = 0;
	int textalignment = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);
	argReader.ReadString(text);
	argReader.ReadNumber(materialindex);
	argReader.ReadNumber(materialsize, OBJECT_MATERIAL_SIZE_256x128);
	argReader.ReadString(fontface, "Arial");
	argReader.ReadNumber(fontsize, 24);
	argReader.ReadBool(bold, true);
	argReader.ReadNumber(fontcolor, 0xFFFFFFFF);
	argReader.ReadNumber(backcolor, 0);
	argReader.ReadNumber(textalignment, 0);

	SetPlayerObjectMaterialText(playerid, objectid, text.c_str(), materialindex, materialsize, fontface.c_str(), fontsize, bold, fontcolor, backcolor, textalignment);

	return 0;
}

LUA_FUNCTION setObjectsDefaultCameraCol(lua_State *L)
{
	bool disable;

	ArgReader argReader(L);
	argReader.ReadBool(disable);

	SetObjectsDefaultCameraCol(disable);

	return 0;
}

LUA_FUNCTION createVehicle(lua_State *L)
{
	int vehicletype;
	float x;
	float y;
	float z;
	float rotation;
	int color1;
	int color2;
	int respawn_delay;
	bool addsiren = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicletype);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);
	argReader.ReadFloat(rotation);
	argReader.ReadNumber(color1);
	argReader.ReadNumber(color2);
	argReader.ReadNumber(respawn_delay);
	argReader.ReadBool(addsiren, false);

	int vehicleid = CreateVehicle(vehicletype, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
	lua_pushnumber(L, vehicleid);

	return 1;
}

LUA_FUNCTION destroyVehicle(lua_State *L)
{
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	bool success = DestroyVehicle(vehicleid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION isVehicleStreamedIn(lua_State *L)
{
	int vehicleid;
	int forplayerid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(forplayerid);

	bool isStreamedIn = IsVehicleStreamedIn(vehicleid, forplayerid);
	lua_pushboolean(L, isStreamedIn);

	return 1;
}

LUA_FUNCTION getVehiclePos(lua_State *L)
{
	int vehicleid;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	GetVehiclePos(vehicleid, &x, &y, &z);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);

	return 3;
}

LUA_FUNCTION setVehiclePos(lua_State *L)
{
	int vehicleid;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);

	SetVehiclePos(vehicleid, x, y, z);

	return 0;
}

LUA_FUNCTION getVehicleZAngle(lua_State *L)
{
	int vehicleid;
	float z_angle;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	GetVehicleZAngle(vehicleid, &z_angle);
	lua_pushnumber(L, z_angle);

	return 1;
}

LUA_FUNCTION getVehicleRotationQuat(lua_State *L)
{
	int vehicleid;
	float w;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	GetVehicleRotationQuat(vehicleid, &w, &x, &y, &z);
	lua_pushnumber(L, w);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);

	return 4;
}

LUA_FUNCTION getVehicleDistanceFromPoint(lua_State *L)
{
	int vehicleid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);

	float distance = GetVehicleDistanceFromPoint(vehicleid, X, Y, Z);
	lua_pushnumber(L, distance);

	return 1;
}

LUA_FUNCTION setVehicleZAngle(lua_State *L)
{
	int vehicleid;
	float z_angle;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadFloat(z_angle);

	bool success = SetVehicleZAngle(vehicleid, z_angle);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION setVehicleParamsForPlayer(lua_State *L)
{
	int vehicleid;
	int playerid;
	bool objective;
	bool doorslocked;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(playerid);
	argReader.ReadBool(objective);
	argReader.ReadBool(doorslocked);

	bool success = SetVehicleParamsForPlayer(vehicleid, playerid, objective, doorslocked);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION manualVehicleEngineAndLights(lua_State *L)
{
	ManualVehicleEngineAndLights();
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION setVehicleParamsEx(lua_State *L)
{
	int vehicleid;
	bool engine;
	bool lights;
	bool alarm;
	bool doors;
	bool bonnet;
	bool boot;
	bool objective;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadBool(engine);
	argReader.ReadBool(lights);
	argReader.ReadBool(alarm);
	argReader.ReadBool(doors);
	argReader.ReadBool(bonnet);
	argReader.ReadBool(boot);
	argReader.ReadBool(objective);

	bool success = SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getVehicleParamsEx(lua_State *L)
{
	int vehicleid;
	int engine;
	int lights;
	int alarm;
	int doors;
	int bonnet;
	int boot;
	int objective;
	
	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	GetVehicleParamsEx(vehicleid, &engine, &lights, &alarm, &doors, &bonnet, &boot, &objective);
	lua_pushnumber(L, engine);
	lua_pushnumber(L, lights);
	lua_pushnumber(L, alarm);
	lua_pushnumber(L, doors);
	lua_pushnumber(L, bonnet);
	lua_pushnumber(L, boot);
	lua_pushnumber(L, objective);

	return 7;
}

LUA_FUNCTION getVehicleParamsSirenState(lua_State *L)
{
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	int sirenState = GetVehicleParamsSirenState(vehicleid);
	lua_pushnumber(L, sirenState);

	return 1;
}

LUA_FUNCTION setVehicleParamsCarDoors(lua_State *L)
{
	int vehicleid;
	bool driver;
	bool passenger;
	bool backleft;
	bool backright;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadBool(driver);
	argReader.ReadBool(passenger);
	argReader.ReadBool(backleft);
	argReader.ReadBool(backright);

	SetVehicleParamsCarDoors(vehicleid, driver, passenger, backleft, backright);

	return 0;
}

LUA_FUNCTION getVehicleParamsCarDoors(lua_State *L)
{
	int vehicleid;
	int driver;
	int passenger;
	int backleft;
	int backright;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	GetVehicleParamsCarDoors(vehicleid, &driver, &passenger, &backleft, &backright);
	lua_pushnumber(L, driver);
	lua_pushnumber(L, passenger);
	lua_pushnumber(L, backleft);
	lua_pushnumber(L, backright);

	return 4;
}

LUA_FUNCTION setVehicleParamsCarWindows(lua_State *L)
{
	int vehicleid;
	bool driver;
	bool passenger;
	bool backleft;
	bool backright;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadBool(driver);
	argReader.ReadBool(passenger);
	argReader.ReadBool(backleft);
	argReader.ReadBool(backright);

	SetVehicleParamsCarWindows(vehicleid, driver, passenger, backleft, backright);

	return 0;
}

LUA_FUNCTION getVehicleParamsCarWindows(lua_State *L)
{
	int vehicleid;
	int driver;
	int passenger;
	int backleft;
	int backright;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	GetVehicleParamsCarWindows(vehicleid, &driver, &passenger, &backleft, &backright);
	lua_pushnumber(L, driver);
	lua_pushnumber(L, passenger);
	lua_pushnumber(L, backleft);
	lua_pushnumber(L, backright);

	return 4;
}

LUA_FUNCTION setVehicleToRespawn(lua_State *L)
{
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	bool success = SetVehicleToRespawn(vehicleid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION linkVehicleToInterior(lua_State *L)
{
	int vehicleid;
	int interiorid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(interiorid);

	bool success = LinkVehicleToInterior(vehicleid, interiorid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION addVehicleComponent(lua_State *L)
{
	int vehicleid;
	int componentid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(componentid);

	bool success = AddVehicleComponent(vehicleid, componentid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION removeVehicleComponent(lua_State *L)
{
	int vehicleid;
	int componentid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(componentid);

	RemoveVehicleComponent(vehicleid, componentid);

	return 0;
}

LUA_FUNCTION changeVehicleColor(lua_State *L)
{
	int vehicleid;
	int color1;
	int color2;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(color1);
	argReader.ReadNumber(color2);

	bool success = ChangeVehicleColor(vehicleid, color1, color2);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION changeVehiclePaintjob(lua_State *L)
{
	int vehicleid;
	int paintjobid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(paintjobid);

	ChangeVehiclePaintjob(vehicleid, paintjobid);
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION setVehicleHealth(lua_State *L)
{
	int vehicleid;
	float health;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadFloat(health);

	bool success = SetVehicleHealth(vehicleid, health);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getVehicleHealth(lua_State *L)
{
	int vehicleid;
	float health;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	GetVehicleHealth(vehicleid, &health);
	lua_pushnumber(L, health);

	return 1;
}

LUA_FUNCTION attachTrailerToVehicle(lua_State *L)
{
	int trailerid;
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(trailerid);
	argReader.ReadNumber(vehicleid);

	AttachTrailerToVehicle(trailerid, vehicleid);
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION detachTrailerFromVehicle(lua_State *L)
{
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	DetachTrailerFromVehicle(vehicleid);

	return 0;
}

LUA_FUNCTION isTrailerAttachedToVehicle(lua_State *L)
{
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	bool isTrailerAttached = IsTrailerAttachedToVehicle(vehicleid);
	lua_pushboolean(L, isTrailerAttached);

	return 1;
}

LUA_FUNCTION getVehicleTrailer(lua_State *L)
{
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	int trailerid = GetVehicleTrailer(vehicleid);
	lua_pushnumber(L, trailerid);

	return 1;
}

LUA_FUNCTION setVehicleNumberPlate(lua_State *L)
{
	int vehicleid;
	std::string numberplate;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadString(numberplate);

	bool success = SetVehicleNumberPlate(vehicleid, numberplate.c_str());
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getVehicleModel(lua_State *L)
{
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	int vehicleModel = GetVehicleModel(vehicleid);
	lua_pushnumber(L, vehicleModel);

	return 1;
}

LUA_FUNCTION getVehicleComponentInSlot(lua_State *L)
{
	int vehicleid;
	int slot;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(slot);

	int component = GetVehicleComponentInSlot(vehicleid, slot);
	lua_pushnumber(L, component);

	return 1;
}

LUA_FUNCTION getVehicleComponentType(lua_State *L)
{
	int component;

	ArgReader argReader(L);
	argReader.ReadNumber(component);

	int componentSlot = GetVehicleComponentType(component);
	lua_pushnumber(L, componentSlot);

	return 1;
}

LUA_FUNCTION repairVehicle(lua_State *L)
{
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	bool success = RepairVehicle(vehicleid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getVehicleVelocity(lua_State *L)
{
	int vehicleid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	GetVehicleVelocity(vehicleid, &X, &Y, &Z);
	lua_pushnumber(L, X);
	lua_pushnumber(L, Y);
	lua_pushnumber(L, Z);

	return 3;
}

LUA_FUNCTION setVehicleVelocity(lua_State *L)
{
	int vehicleid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);

	bool success = SetVehicleVelocity(vehicleid, X, Y, Z);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION setVehicleAngularVelocity(lua_State *L)
{
	int vehicleid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);

	SetVehicleAngularVelocity(vehicleid, X, Y, Z);

	return 0;
}

LUA_FUNCTION getVehicleDamageStatus(lua_State *L)
{
	int vehicleid;
	int panels;
	int doors;
	int lights;
	int tires;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	GetVehicleDamageStatus(vehicleid, &panels, &doors, &lights, &tires);
	lua_pushnumber(L, panels);
	lua_pushnumber(L, doors);
	lua_pushnumber(L, lights);
	lua_pushnumber(L, tires);

	return 4;
}

LUA_FUNCTION updateVehicleDamageStatus(lua_State *L)
{
	int vehicleid;
	int panels;
	int doors;
	int lights;
	int tires;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(panels);
	argReader.ReadNumber(doors);
	argReader.ReadNumber(lights);
	argReader.ReadNumber(tires);

	UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, tires);

	return 0;
}

LUA_FUNCTION getVehicleModelInfo(lua_State *L)
{
	int vehiclemodel;
	int infotype;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(vehiclemodel);
	argReader.ReadNumber(infotype);

	GetVehicleModelInfo(vehiclemodel, infotype, &X, &Y, &Z);
	lua_pushnumber(L, X);
	lua_pushnumber(L, Y);
	lua_pushnumber(L, Z);

	return 3;
}

LUA_FUNCTION setVehicleVirtualWorld(lua_State *L)
{
	int vehicleid;
	int worldid;
	
	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(worldid);

	SetVehicleVirtualWorld(vehicleid, worldid);

	return 0;
}

LUA_FUNCTION getVehicleVirtualWorld(lua_State *L)
{
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(vehicleid);

	int virtualWorld = GetVehicleVirtualWorld(vehicleid);
	lua_pushnumber(L, virtualWorld);

	return 1;
}

LUA_FUNCTION setSpawnInfo(lua_State *L)
{
	int playerid;
	int team;
	int skin;
	float x;
	float y;
	float z;
	float rotation;
	int weapon1;
	int weapon1_ammo;
	int weapon2;
	int weapon2_ammo;
	int weapon3;
	int weapon3_ammo;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(team);
	argReader.ReadNumber(skin);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);
	argReader.ReadFloat(rotation);
	argReader.ReadNumber(weapon1);
	argReader.ReadNumber(weapon1_ammo);
	argReader.ReadNumber(weapon2);
	argReader.ReadNumber(weapon2_ammo);
	argReader.ReadNumber(weapon3);
	argReader.ReadNumber(weapon3_ammo);

	SetSpawnInfo(playerid, team, skin, x, y, z, rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);

	return 0;
}

LUA_FUNCTION spawnPlayer(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	SpawnPlayer(playerid);

	return 0;
}

LUA_FUNCTION setPlayerPos(lua_State *L)
{
	int playerid;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);

	bool success = SetPlayerPos(playerid, x, y, z);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION setPlayerPosFindZ(lua_State *L)
{
	int playerid;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);

	SetPlayerPosFindZ(playerid, x, y, z);

	return 0;
}

LUA_FUNCTION getPlayerPos(lua_State *L)
{
	int playerid;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerPos(playerid, &x, &y, &z);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);

	return 3;
}

LUA_FUNCTION setPlayerFacingAngle(lua_State *L)
{
	int playerid;
	float ang;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(ang);

	bool success = SetPlayerFacingAngle(playerid, ang);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerFacingAngle(lua_State *L)
{
	int playerid;
	float ang;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerFacingAngle(playerid, &ang);
	lua_pushnumber(L, ang);

	return 1;
}

LUA_FUNCTION isPlayerInRangeOfPoint(lua_State *L)
{
	int playerid;
	float range;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(range);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);

	bool isPlayerInRange = IsPlayerInRangeOfPoint(playerid, range, x, y, z);
	lua_pushboolean(L, isPlayerInRange);

	return 1;
}

LUA_FUNCTION getPlayerDistanceFromPoint(lua_State *L)
{
	int playerid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);

	float distanceFromPoint = GetPlayerDistanceFromPoint(playerid, X, Y, Z);
	lua_pushnumber(L, distanceFromPoint);

	return 1;
}

LUA_FUNCTION isPlayerStreamedIn(lua_State *L)
{
	int playerid;
	int forplayerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(forplayerid);

	bool isStreamedIn = IsPlayerStreamedIn(playerid, forplayerid);
	lua_pushboolean(L, isStreamedIn);

	return 1;
}

LUA_FUNCTION setPlayerInterior(lua_State *L)
{
	int playerid;
	int interiorid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(interiorid);

	bool success = SetPlayerInterior(playerid, interiorid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerInterior(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int interiorid = GetPlayerInterior(playerid);
	lua_pushnumber(L, interiorid);

	return 1;
}

LUA_FUNCTION setPlayerHealth(lua_State *L)
{
	int playerid;
	float health;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(health);

	SetPlayerHealth(playerid, health);

	return 0;
}

LUA_FUNCTION getPlayerHealth(lua_State *L)
{
	int playerid;
	float health;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerHealth(playerid, &health);
	lua_pushnumber(L, health);

	return 1;
}

LUA_FUNCTION setPlayerArmour(lua_State *L)
{
	int playerid;
	float armour;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(armour);

	bool success = SetPlayerArmour(playerid, armour);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerArmour(lua_State *L)
{
	int playerid;
	float armour;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerArmour(playerid, &armour);
	lua_pushnumber(L, armour);

	return 1;
}

LUA_FUNCTION setPlayerAmmo(lua_State *L)
{
	int playerid;
	int weaponslot;
	int ammo;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(weaponslot);
	argReader.ReadNumber(ammo);

	bool success = SetPlayerAmmo(playerid, weaponslot, ammo);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerAmmo(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int ammo = GetPlayerAmmo(playerid);
	lua_pushnumber(L, ammo);

	return 1;
}

LUA_FUNCTION getPlayerWeaponState(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int weaponState = GetPlayerWeaponState(playerid);
	lua_pushnumber(L, weaponState);

	return 1;
}

LUA_FUNCTION getPlayerTargetPlayer(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int targetPlayer = GetPlayerTargetPlayer(playerid);
	lua_pushnumber(L, targetPlayer);

	return 1;
}

LUA_FUNCTION getPlayerTargetActor(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int targetActor = GetPlayerTargetActor(playerid);
	lua_pushnumber(L, targetActor);

	return 1;
}

LUA_FUNCTION setPlayerTeam(lua_State *L)
{
	int playerid;
	int teamid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(teamid);

	SetPlayerTeam(playerid, teamid);

	return 0;
}

LUA_FUNCTION getPlayerTeam(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int teamid = GetPlayerTeam(playerid);
	lua_pushnumber(L, teamid);

	return 1;
}

LUA_FUNCTION setPlayerScore(lua_State *L)
{
	int playerid;
	int score;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(score);

	bool success = SetPlayerScore(playerid, score);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerScore(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int score = GetPlayerScore(playerid);
	lua_pushnumber(L, score);

	return 1;
}

LUA_FUNCTION getPlayerDrunkLevel(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int drunkLevel = GetPlayerDrunkLevel(playerid);
	lua_pushnumber(L, drunkLevel);

	return 1;
}

LUA_FUNCTION setPlayerDrunkLevel(lua_State *L)
{
	int playerid;
	int level;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(level);

	bool success = SetPlayerDrunkLevel(playerid, level);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION setPlayerColor(lua_State *L)
{
	int playerid;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(color);

	SetPlayerColor(playerid, color);

	return 0;
}

LUA_FUNCTION getPlayerColor(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int color = GetPlayerColor(playerid);
	lua_pushnumber(L, color);

	return 1;
}

LUA_FUNCTION setPlayerSkin(lua_State *L)
{
	int playerid;
	int skinid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(skinid);

	bool success = SetPlayerSkin(playerid, skinid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerSkin(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int skinid = GetPlayerSkin(playerid);
	lua_pushnumber(L, skinid);

	return 1;
}

LUA_FUNCTION givePlayerWeapon(lua_State *L)
{
	int playerid;
	int weaponid;
	int ammo;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(weaponid);
	argReader.ReadNumber(ammo);

	GivePlayerWeapon(playerid, weaponid, ammo);

	return 0;
}

LUA_FUNCTION resetPlayerWeapons(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	ResetPlayerWeapons(playerid);

	return 0;
}

LUA_FUNCTION setPlayerArmedWeapon(lua_State *L)
{
	int playerid;
	int weaponid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(weaponid);

	bool success = SetPlayerArmedWeapon(playerid, weaponid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerWeaponData(lua_State *L)
{
	int playerid;
	int slot;
	int weapons;
	int ammo;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(slot);

	GetPlayerWeaponData(playerid, slot, &weapons, &ammo);
	lua_pushnumber(L, weapons);
	lua_pushnumber(L, ammo);

	return 2;
}

LUA_FUNCTION givePlayerMoney(lua_State *L)
{
	int playerid;
	int money;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(money);

	bool success = GivePlayerMoney(playerid, money);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION resetPlayerMoney(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	ResetPlayerMoney(playerid);

	return 0;
}

LUA_FUNCTION setPlayerName(lua_State *L)
{
	int playerid;
	std::string name;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(name);

	int namereturn = SetPlayerName(playerid, name.c_str());
	lua_pushnumber(L, namereturn);

	return 1;
}

LUA_FUNCTION getPlayerMoney(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int money = GetPlayerMoney(playerid);
	lua_pushnumber(L, money);

	return 1;
}

LUA_FUNCTION getPlayerState(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int state = GetPlayerState(playerid);
	lua_pushnumber(L, state);

	return 1;
}

LUA_FUNCTION getPlayerIp(lua_State *L)
{
	int playerid;
	char ip[16];

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerIp(playerid, ip, sizeof(ip));
	lua_pushstring(L, ip);

	return 1;
}

LUA_FUNCTION getPlayerPing(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int ping = GetPlayerPing(playerid);
	lua_pushnumber(L, ping);

	return 1;
}

LUA_FUNCTION getPlayerWeapon(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int weapon = GetPlayerWeapon(playerid);
	lua_pushnumber(L, weapon);

	return 1;
}

LUA_FUNCTION getPlayerKeys(lua_State *L)
{
	int playerid;
	int keys;
	int updown;
	int leftright;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerKeys(playerid, &keys, &updown, &leftright);
	lua_pushnumber(L, keys);
	lua_pushnumber(L, updown);
	lua_pushnumber(L, leftright);

	return 3;
}

LUA_FUNCTION getPlayerName(lua_State *L)
{
	int playerid;
	char playername[MAX_PLAYER_NAME];

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerName(playerid, playername, sizeof(playername));
	lua_pushstring(L, playername);

	return 1;
}

LUA_FUNCTION setPlayerTime(lua_State *L)
{
	int playerid;
	int hour;
	int minute;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(hour);
	argReader.ReadNumber(minute);

	bool success = SetPlayerTime(playerid, hour, minute);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerTime(lua_State *L)
{
	int playerid;
	int hour;
	int minute;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerTime(playerid, &hour, &minute);
	lua_pushnumber(L, hour);
	lua_pushnumber(L, minute);

	return 2;
}

LUA_FUNCTION togglePlayerClock(lua_State *L)
{
	int playerid;
	bool toggle;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadBool(toggle);

	bool success = TogglePlayerClock(playerid, toggle);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION setPlayerWeather(lua_State *L)
{
	int playerid;
	int weather;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(weather);

	SetPlayerWeather(playerid, weather);

	return 0;
}

LUA_FUNCTION forceClassSelection(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	ForceClassSelection(playerid);

	return 0;
}

LUA_FUNCTION setPlayerWantedLevel(lua_State *L)
{
	int playerid;
	int level;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(level);

	bool success = SetPlayerWantedLevel(playerid, level);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerWantedLevel(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int wantedLevel = GetPlayerWantedLevel(playerid);
	lua_pushnumber(L, wantedLevel);

	return 1;
}

LUA_FUNCTION setPlayerFightingStyle(lua_State *L)
{
	int playerid;
	int style;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(style);

	SetPlayerFightingStyle(playerid, style);

	return 0;
}

LUA_FUNCTION getPlayerFightingStyle(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int fightingStyle = GetPlayerFightingStyle(playerid);
	lua_pushnumber(L, fightingStyle);

	return 1;
}

LUA_FUNCTION setPlayerVelocity(lua_State *L)
{
	int playerid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);

	SetPlayerVelocity(playerid, X, Y, Z);

	return 0;
}

LUA_FUNCTION getPlayerVelocity(lua_State *L)
{
	int playerid;
	float X;
	float Y;
	float Z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerVelocity(playerid, &X, &Y, &Z);
	lua_pushnumber(L, X);
	lua_pushnumber(L, Y);
	lua_pushnumber(L, Z);

	return 3;
}

LUA_FUNCTION playCrimeReportForPlayer(lua_State *L)
{
	int playerid;
	int suspectid;
	int crime;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(suspectid);
	argReader.ReadNumber(crime);

	PlayCrimeReportForPlayer(playerid, suspectid, crime);

	return 0;
}

LUA_FUNCTION playAudioStreamForPlayer(lua_State *L)
{
	int playerid;
	std::string url;
	float posX = 0.0;
	float posY = 0.0;
	float posZ = 0.0;
	float distance = 50.0;
	bool usepos = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(url);
	argReader.ReadFloat(posX, 0.0);
	argReader.ReadFloat(posY, 0.0);
	argReader.ReadFloat(posZ, 0.0);
	argReader.ReadFloat(distance, 50.0);
	argReader.ReadBool(usepos, false);

	bool success = PlayAudioStreamForPlayer(playerid, url.c_str(), posX, posY, posZ, distance, usepos);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION stopAudioStreamForPlayer(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	StopAudioStreamForPlayer(playerid);

	return 0;
}

LUA_FUNCTION setPlayerShopName(lua_State *L)
{
	int playerid;
	std::string shopname;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(shopname);

	SetPlayerShopName(playerid, shopname.c_str());

	return 0;
}

LUA_FUNCTION setPlayerSkillLevel(lua_State *L)
{
	int playerid;
	int skill;
	int level;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(skill);
	argReader.ReadNumber(level);

	SetPlayerSkillLevel(playerid, skill, level);

	return 0;
}

LUA_FUNCTION getPlayerSurfingVehicleID(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int vehicleid = GetPlayerSurfingVehicleID(playerid);
	lua_pushnumber(L, vehicleid);

	return 1;
}

LUA_FUNCTION getPlayerSurfingObjectID(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int objectid = GetPlayerSurfingObjectID(playerid);
	lua_pushnumber(L, objectid);

	return 1;
}

LUA_FUNCTION removeBuildingForPlayer(lua_State *L)
{
	int playerid;
	int modelid;
	float fX;
	float fY;
	float fZ;
	float fRadius;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(modelid);
	argReader.ReadFloat(fX);
	argReader.ReadFloat(fY);
	argReader.ReadFloat(fZ);
	argReader.ReadFloat(fRadius);

	RemoveBuildingForPlayer(playerid, modelid, fX, fY, fZ, fRadius);

	return 0;
}

LUA_FUNCTION getPlayerLastShotVectors(lua_State *L)
{
	int playerid;
	float fOriginX;
	float fOriginY;
	float fOriginZ;
	float fHitPosX;
	float fHitPosY;
	float fHitPosZ;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerLastShotVectors(playerid, &fOriginX, &fOriginY, &fOriginZ, &fHitPosX, &fHitPosY, &fHitPosZ);
	lua_pushnumber(L, fOriginX);
	lua_pushnumber(L, fOriginY);
	lua_pushnumber(L, fOriginZ);
	lua_pushnumber(L, fHitPosX);
	lua_pushnumber(L, fHitPosY);
	lua_pushnumber(L, fHitPosZ);

	return 6;
}

LUA_FUNCTION setPlayerAttachedObject(lua_State *L)
{
	int playerid;
	int index;
	int modelid;
	int bone;
	float fOffsetX = 0.0;
	float fOffsetY = 0.0;
	float fOffsetZ = 0.0;
	float fRotX = 0.0;
	float fRotY = 0.0;
	float fRotZ = 0.0;
	float fScaleX = 1.0;
	float fScaleY = 1.0;
	float fScaleZ = 1.0;
	int materialcolor1 = 0;
	int materialcolor2 = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(index);
	argReader.ReadNumber(modelid);
	argReader.ReadNumber(bone);
	argReader.ReadFloat(fOffsetX, 0.0);
	argReader.ReadFloat(fOffsetY, 0.0);
	argReader.ReadFloat(fOffsetZ, 0.0);
	argReader.ReadFloat(fRotX, 0.0);
	argReader.ReadFloat(fRotY, 0.0);
	argReader.ReadFloat(fRotZ, 0.0);
	argReader.ReadFloat(fScaleX, 1.0);
	argReader.ReadFloat(fScaleY, 1.0);
	argReader.ReadFloat(fScaleZ, 1.0);
	argReader.ReadNumber(materialcolor1, 0);
	argReader.ReadNumber(materialcolor2, 0);

	bool success = SetPlayerAttachedObject(playerid, index, modelid, bone, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ, materialcolor1, materialcolor2);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION removePlayerAttachedObject(lua_State *L)
{
	int playerid;
	int index;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(index);

	bool success = RemovePlayerAttachedObject(playerid, index);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION isPlayerAttachedObjectSlotUsed(lua_State *L)
{
	int playerid;
	int index;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(index);

	bool isSlotUsed = IsPlayerAttachedObjectSlotUsed(playerid, index);
	lua_pushboolean(L, isSlotUsed);

	return 1;
}

LUA_FUNCTION editAttachedObject(lua_State *L)
{
	int playerid;
	int index;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(index);

	bool success = EditAttachedObject(playerid, index);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION createPlayerTextDraw(lua_State *L)
{
	int playerid;
	float x;
	float y;
	std::string text;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadString(text);

	int playerTextDraw = CreatePlayerTextDraw(playerid, x, y, text.c_str());
	lua_pushnumber(L, playerTextDraw);

	return 1;
}

LUA_FUNCTION playerTextDrawDestroy(lua_State *L)
{
	int playerid;
	int text;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);

	PlayerTextDrawDestroy(playerid, text);

	return 0;
}

LUA_FUNCTION playerTextDrawLetterSize(lua_State *L)
{
	int playerid;
	int text;
	float x;
	float y;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);

	PlayerTextDrawLetterSize(playerid, text, x, y);

	return 0;
}

LUA_FUNCTION playerTextDrawTextSize(lua_State *L)
{
	int playerid;
	int text;
	float x;
	float y;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);

	PlayerTextDrawTextSize(playerid, text, x, y);

	return 0;
}

LUA_FUNCTION playerTextDrawAlignment(lua_State *L)
{
	int playerid;
	int text;
	int alignment;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadNumber(alignment);

	PlayerTextDrawAlignment(playerid, text, alignment);

	return 0;
}

LUA_FUNCTION playerTextDrawColor(lua_State *L)
{
	int playerid;
	int text;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadNumber(color);

	PlayerTextDrawColor(playerid, text, color);

	return 0;
}

LUA_FUNCTION playerTextDrawUseBox(lua_State *L)
{
	int playerid;
	int text;
	bool use;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadBool(use);

	PlayerTextDrawUseBox(playerid, text, use);

	return 0;
}

LUA_FUNCTION playerTextDrawBoxColor(lua_State *L)
{
	int playerid;
	int text;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadNumber(color);

	PlayerTextDrawBoxColor(playerid, text, color);

	return 0;
}

LUA_FUNCTION playerTextDrawSetShadow(lua_State *L)
{
	int playerid;
	int text;
	int size;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadNumber(size);

	bool success = PlayerTextDrawSetShadow(playerid, text, size);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION playerTextDrawSetOutline(lua_State *L)
{
	int playerid;
	int text;
	int size;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadNumber(size);

	PlayerTextDrawSetOutline(playerid, text, size);

	return 0;
}

LUA_FUNCTION playerTextDrawBackgroundColor(lua_State *L)
{
	int playerid;
	int text;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadNumber(color);

	PlayerTextDrawBackgroundColor(playerid, text, color);

	return 0;
}

LUA_FUNCTION playerTextDrawFont(lua_State *L)
{
	int playerid;
	int text;
	int font;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadNumber(font);

	PlayerTextDrawFont(playerid, text, font);

	return 0;
}

LUA_FUNCTION playerTextDrawSetProportional(lua_State *L)
{
	int playerid;
	int text;
	bool set;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadBool(set);

	PlayerTextDrawSetProportional(playerid, text, set);

	return 0;
}

LUA_FUNCTION playerTextDrawSetSelectable(lua_State *L)
{
	int playerid;
	int text;
	bool set;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadBool(set);

	PlayerTextDrawSetSelectable(playerid, text, set);

	return 0;
}

LUA_FUNCTION playerTextDrawShow(lua_State *L)
{
	int playerid;
	int text;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);

	PlayerTextDrawShow(playerid, text);

	return 0;
}

LUA_FUNCTION playerTextDrawHide(lua_State *L)
{
	int playerid;
	int text;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);

	PlayerTextDrawHide(playerid, text);

	return 0;
}

LUA_FUNCTION playerTextDrawSetString(lua_State *L)
{
	int playerid;
	int text;
	std::string string;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadString(string);

	PlayerTextDrawSetString(playerid, text, string.c_str());

	return 0;
}

LUA_FUNCTION playerTextDrawSetPreviewModel(lua_State *L)
{
	int playerid;
	int text;
	int modelindex;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadNumber(modelindex);

	bool success = PlayerTextDrawSetPreviewModel(playerid, text, modelindex);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION playerTextDrawSetPreviewRot(lua_State *L)
{
	int playerid;
	int text;
	float fRotX;
	float fRotY;
	float fRotZ;
	float fZoom = 1.0;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadFloat(fRotX);
	argReader.ReadFloat(fRotY);
	argReader.ReadFloat(fRotZ);
	argReader.ReadFloat(fZoom, 1.0);

	PlayerTextDrawSetPreviewRot(playerid, text, fRotX, fRotY, fRotZ, fZoom);

	return 0;
}

LUA_FUNCTION playerTextDrawSetPreviewVehCol(lua_State *L)
{
	int playerid;
	int text;
	int color1;
	int color2;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(text);
	argReader.ReadNumber(color1);
	argReader.ReadNumber(color2);

	PlayerTextDrawSetPreviewVehCol(playerid, text, color1, color2);

	return 0;
}

LUA_FUNCTION setPVarInt(lua_State *L)
{
	int playerid;
	std::string varname;
	int int_value;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(varname);
	argReader.ReadNumber(int_value);

	bool success = SetPVarInt(playerid, varname.c_str(), int_value);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPVarInt(lua_State *L)
{
	int playerid;
	std::string varname;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(varname);

	int int_value = GetPVarInt(playerid, varname.c_str());
	lua_pushnumber(L, int_value);

	return 1;
}

LUA_FUNCTION setPVarString(lua_State *L)
{
	int playerid;
	std::string varname;
	std::string string_value;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(varname);
	argReader.ReadString(string_value);

	SetPVarString(playerid, varname.c_str(), string_value.c_str());

	return 0;
}

LUA_FUNCTION getPVarString(lua_State *L)
{
	int playerid;
	std::string varname;
	char string_return[128];

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(varname);

	GetPVarString(playerid, varname.c_str(), string_return, sizeof(string_return));
	lua_pushstring(L, string_return);

	return 1;
}

LUA_FUNCTION setPVarFloat(lua_State *L)
{
	int playerid;
	std::string varname;
	float float_value;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(varname);
	argReader.ReadFloat(float_value);

	bool success = SetPVarFloat(playerid, varname.c_str(), float_value);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPVarFloat(lua_State *L)
{
	int playerid;
	std::string varname;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(varname);

	float float_value = GetPVarFloat(playerid, varname.c_str());
	lua_pushnumber(L, float_value);

	return 1;
}

LUA_FUNCTION deletePVar(lua_State *L)
{
	int playerid;
	std::string varname;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(varname);

	bool success = DeletePVar(playerid, varname.c_str());
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPVarsUpperIndex(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int upperIndex = GetPVarsUpperIndex(playerid);
	lua_pushnumber(L, upperIndex);

	return 1;
}

LUA_FUNCTION getPVarNameAtIndex(lua_State *L)
{
	int playerid;
	int index;
	char ret_varname[41];

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(index);

	GetPVarNameAtIndex(playerid, index, ret_varname, sizeof(ret_varname));
	lua_pushstring(L, ret_varname);

	return 1;
}

LUA_FUNCTION getPVarType(lua_State *L)
{
	int playerid;
	std::string varname;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(varname);

	int pvarType = GetPVarType(playerid, varname.c_str());
	lua_pushnumber(L, pvarType);

	return 1;
}

LUA_FUNCTION setPlayerChatBubble(lua_State *L)
{
	int playerid;
	std::string text;
	int color;
	float drawdistance;
	int expiretime;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(text);
	argReader.ReadNumber(color);
	argReader.ReadFloat(drawdistance);
	argReader.ReadNumber(expiretime);

	SetPlayerChatBubble(playerid, text.c_str(), color, drawdistance, expiretime);

	return 0;
}

LUA_FUNCTION putPlayerInVehicle(lua_State *L)
{
	int playerid;
	int vehicleid;
	int seatid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(vehicleid);
	argReader.ReadNumber(seatid);

	PutPlayerInVehicle(playerid, vehicleid, seatid);

	return 0;
}

LUA_FUNCTION getPlayerVehicleID(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int vehicleid = GetPlayerVehicleID(playerid);
	lua_pushnumber(L, vehicleid);

	return 1;
}

LUA_FUNCTION getPlayerVehicleSeat(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int vehicleSeat = GetPlayerVehicleSeat(playerid);
	lua_pushnumber(L, vehicleSeat);

	return 1;
}

LUA_FUNCTION removePlayerFromVehicle(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	RemovePlayerFromVehicle(playerid);

	return 0;
}

LUA_FUNCTION togglePlayerControllable(lua_State *L)
{
	int playerid;
	bool toggle;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadBool(toggle);

	bool success = TogglePlayerControllable(playerid, toggle);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION playerPlaySound(lua_State *L)
{
	int playerid;
	int soundid;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(soundid);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);

	PlayerPlaySound(playerid, soundid, x, y, z);

	return 0;
}

LUA_FUNCTION applyAnimation(lua_State *L)
{
	int playerid;
	std::string animlib;
	std::string animname;
	float fDelta;
	bool loop;
	bool lockx;
	bool locky;
	bool freeze;
	int time;
	bool forcesync = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadString(animlib);
	argReader.ReadString(animname);
	argReader.ReadFloat(fDelta);
	argReader.ReadBool(loop);
	argReader.ReadBool(lockx);
	argReader.ReadBool(locky);
	argReader.ReadBool(freeze);
	argReader.ReadNumber(time);
	argReader.ReadBool(forcesync, false);

	ApplyAnimation(playerid, animlib.c_str(), animname.c_str(), fDelta, loop, lockx, locky, freeze, time, forcesync);
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION clearAnimations(lua_State *L)
{
	int playerid;
	bool forcesync = 0;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadBool(forcesync, false);

	ClearAnimations(playerid, forcesync);
	lua_pushboolean(L, true);

	return 1;
}

LUA_FUNCTION getPlayerAnimationIndex(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int animationIndex = GetPlayerAnimationIndex(playerid);
	lua_pushnumber(L, animationIndex);

	return 1;
}

LUA_FUNCTION getAnimationName(lua_State *L)
{
	int index;
	char animlib[32];
	char animname[32];

	ArgReader argReader(L);
	argReader.ReadNumber(index);

	GetAnimationName(index, animlib, sizeof(animlib), animname, sizeof(animname));
	lua_pushstring(L, animlib);
	lua_pushstring(L, animname);

	return 2;
}

LUA_FUNCTION getPlayerSpecialAction(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int specialAction = GetPlayerSpecialAction(playerid);
	lua_pushnumber(L, specialAction);

	return 1;
}

LUA_FUNCTION setPlayerSpecialAction(lua_State *L)
{
	int playerid;
	int actionid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(actionid);

	SetPlayerSpecialAction(playerid, actionid);

	return 0;
}

LUA_FUNCTION disableRemoteVehicleCollisions(lua_State *L)
{
	int playerid;
	bool disable;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadBool(disable);

	DisableRemoteVehicleCollisions(playerid, disable);

	return 0;
}

LUA_FUNCTION setPlayerCheckpoint(lua_State *L)
{
	int playerid;
	float x;
	float y;
	float z;
	float size;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);
	argReader.ReadFloat(size);

	bool success = SetPlayerCheckpoint(playerid, x, y, z, size);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION disablePlayerCheckpoint(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	bool success = DisablePlayerCheckpoint(playerid);
	lua_pushboolean(L, success);

	return 0;
}

LUA_FUNCTION setPlayerRaceCheckpoint(lua_State *L)
{
	int playerid;
	int type;
	float x;
	float y;
	float z;
	float nextx;
	float nexty;
	float nextz;
	float size;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(type);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);
	argReader.ReadFloat(nextx);
	argReader.ReadFloat(nexty);
	argReader.ReadFloat(nextz);
	argReader.ReadFloat(size);

	SetPlayerRaceCheckpoint(playerid, type, x, y, z, nextx, nexty, nextz, size);

	return 0;
}

LUA_FUNCTION disablePlayerRaceCheckpoint(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	DisablePlayerRaceCheckpoint(playerid);

	return 0;
}

LUA_FUNCTION setPlayerWorldBounds(lua_State *L)
{
	int playerid;
	float x_max;
	float x_min;
	float y_max;
	float y_min;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(x_max);
	argReader.ReadFloat(x_min);
	argReader.ReadFloat(y_max);
	argReader.ReadFloat(y_min);

	SetPlayerWorldBounds(playerid, x_max, x_min, y_max, y_min);

	return 0;
}

LUA_FUNCTION setPlayerMarkerForPlayer(lua_State *L)
{
	int playerid;
	int showplayerid;
	int color;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(showplayerid);
	argReader.ReadNumber(color);

	SetPlayerMarkerForPlayer(playerid, showplayerid, color);

	return 0;
}

LUA_FUNCTION showPlayerNameTagForPlayer(lua_State *L)
{
	int playerid;
	int showplayerid;
	bool show;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(showplayerid);
	argReader.ReadBool(show);

	ShowPlayerNameTagForPlayer(playerid, showplayerid, show);

	return 0;
}

LUA_FUNCTION setPlayerMapIcon(lua_State *L)
{
	int playerid;
	int iconid;
	float x;
	float y;
	float z;
	int markertype;
	int color;
	int style = MAPICON_LOCAL;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(iconid);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);
	argReader.ReadNumber(markertype);
	argReader.ReadNumber(color);
	argReader.ReadNumber(style, MAPICON_GLOBAL);

	bool success = SetPlayerMapIcon(playerid, iconid, x, y, z, markertype, color, style);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION removePlayerMapIcon(lua_State *L)
{
	int playerid;
	int iconid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(iconid);

	bool success = RemovePlayerMapIcon(playerid, iconid);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION allowPlayerTeleport(lua_State *L)
{
	int playerid;
	bool allow;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadBool(allow);

	AllowPlayerTeleport(playerid, allow);

	return 0;
}

LUA_FUNCTION setPlayerCameraPos(lua_State *L)
{
	int playerid;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);

	bool success = SetPlayerCameraPos(playerid, x, y, z);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION setPlayerCameraLookAt(lua_State *L)
{
	int playerid;
	float x;
	float y;
	float z;
	int cut = CAMERA_CUT;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(x);
	argReader.ReadFloat(y);
	argReader.ReadFloat(z);
	argReader.ReadNumber(cut, CAMERA_CUT);

	bool success = SetPlayerCameraLookAt(playerid, x, y, z, cut);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION setCameraBehindPlayer(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	SetCameraBehindPlayer(playerid);

	return 0;
}

LUA_FUNCTION getPlayerCameraPos(lua_State *L)
{
	int playerid;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerCameraPos(playerid, &x, &y, &z);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);

	return 3;
}

LUA_FUNCTION getPlayerCameraFrontVector(lua_State *L)
{
	int playerid;
	float x;
	float y;
	float z;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	GetPlayerCameraFrontVector(playerid, &x, &y, &z);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);

	return 3;
}

LUA_FUNCTION getPlayerCameraMode(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int cameraMode = GetPlayerCameraMode(playerid);
	lua_pushnumber(L, cameraMode);

	return 1;
}

LUA_FUNCTION enablePlayerCameraTarget(lua_State *L)
{
	int playerid;
	bool enable;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadBool(enable);

	bool success = EnablePlayerCameraTarget(playerid, enable);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION getPlayerCameraTargetObject(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int objectid = GetPlayerCameraTargetObject(playerid);
	lua_pushnumber(L, objectid);

	return 1;
}

LUA_FUNCTION getPlayerCameraTargetVehicle(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int vehicleid = GetPlayerCameraTargetVehicle(playerid);
	lua_pushnumber(L, vehicleid);

	return 1;
}

LUA_FUNCTION getPlayerCameraTargetPlayer(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int targetPlayerid = GetPlayerCameraTargetPlayer(playerid);
	lua_pushnumber(L, targetPlayerid);

	return 1;
}

LUA_FUNCTION getPlayerCameraTargetActor(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int actor = GetPlayerCameraTargetActor(playerid);
	lua_pushnumber(L, actor);

	return 1;
}

LUA_FUNCTION getPlayerCameraAspectRatio(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	float aspectRatio = GetPlayerCameraAspectRatio(playerid);
	lua_pushnumber(L, aspectRatio);

	return 1;
}

LUA_FUNCTION getPlayerCameraZoom(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	float cameraZoom = GetPlayerCameraZoom(playerid);
	lua_pushnumber(L, cameraZoom);

	return 1;
}

LUA_FUNCTION attachCameraToObject(lua_State *L)
{
	int playerid;
	int objectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(objectid);

	AttachCameraToObject(playerid, objectid);

	return 0;
}

LUA_FUNCTION attachCameraToPlayerObject(lua_State *L)
{
	int playerid;
	int playerobjectid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(playerobjectid);

	AttachCameraToPlayerObject(playerid, playerobjectid);

	return 0;
}

LUA_FUNCTION interpolateCameraPos(lua_State *L)
{
	int playerid;
	float FromX;
	float FromY;
	float FromZ;
	float ToX;
	float ToY;
	float ToZ;
	int time;
	int cut = CAMERA_CUT;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(FromX);
	argReader.ReadFloat(FromY);
	argReader.ReadFloat(FromZ);
	argReader.ReadFloat(ToX);
	argReader.ReadFloat(ToY);
	argReader.ReadFloat(ToZ);
	argReader.ReadNumber(time);
	argReader.ReadNumber(cut, CAMERA_CUT);

	InterpolateCameraPos(playerid, FromX, FromY, FromZ, ToX, ToY, ToZ, time, cut);

	return 0;
}

LUA_FUNCTION interpolateCameraLookAt(lua_State *L)
{
	int playerid;
	float FromX;
	float FromY;
	float FromZ;
	float ToX;
	float ToY;
	float ToZ;
	int time;
	int cut = CAMERA_CUT;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(FromX);
	argReader.ReadFloat(FromY);
	argReader.ReadFloat(FromZ);
	argReader.ReadFloat(ToX);
	argReader.ReadFloat(ToY);
	argReader.ReadFloat(ToZ);
	argReader.ReadNumber(time);
	argReader.ReadNumber(cut, CAMERA_CUT);

	InterpolateCameraLookAt(playerid, FromX, FromY, FromZ, ToX, ToY, ToZ, time, cut);

	return 0;
}

LUA_FUNCTION isPlayerConnected(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	bool isConnected = IsPlayerConnected(playerid);
	lua_pushboolean(L, isConnected);

	return 1;
}

LUA_FUNCTION isPlayerInVehicle(lua_State *L)
{
	int playerid;
	int vehicleid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(vehicleid);

	bool inVehicle = IsPlayerInVehicle(playerid, vehicleid);
	lua_pushboolean(L, inVehicle);

	return 1;
}

LUA_FUNCTION isPlayerInAnyVehicle(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	bool inAnyVehicle = IsPlayerInAnyVehicle(playerid);
	lua_pushboolean(L, inAnyVehicle);

	return 1;
}

LUA_FUNCTION isPlayerInCheckpoint(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	bool inCheckpoint = IsPlayerInCheckpoint(playerid);
	lua_pushboolean(L, inCheckpoint);

	return 1;
}

LUA_FUNCTION isPlayerInRaceCheckpoint(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	bool inRaceCheckpoint = IsPlayerInRaceCheckpoint(playerid);
	lua_pushboolean(L, inRaceCheckpoint);

	return 1;
}

LUA_FUNCTION setPlayerVirtualWorld(lua_State *L)
{
	int playerid;
	int worldid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(worldid);

	SetPlayerVirtualWorld(playerid, worldid);

	return 0;
}

LUA_FUNCTION getPlayerVirtualWorld(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	int virtualWorld = GetPlayerVirtualWorld(playerid);
	lua_pushnumber(L, virtualWorld);

	return 1;
}

LUA_FUNCTION enableStuntBonusForPlayer(lua_State *L)
{
	int playerid;
	bool enable;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadBool(enable);

	bool success = EnableStuntBonusForPlayer(playerid, enable);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION enableStuntBonusForAll(lua_State *L)
{
	bool enable;

	ArgReader argReader(L);
	argReader.ReadBool(enable);

	EnableStuntBonusForAll(enable);

	return 0;
}

LUA_FUNCTION togglePlayerSpectating(lua_State *L)
{
	int playerid;
	bool toggle;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadBool(toggle);

	bool success = TogglePlayerSpectating(playerid, toggle);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION playerSpectatePlayer(lua_State *L)
{
	int playerid;
	int targetplayerid;
	int mode = SPECTATE_MODE_NORMAL;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(targetplayerid);
	argReader.ReadNumber(mode, SPECTATE_MODE_NORMAL);

	bool success = PlayerSpectatePlayer(playerid, targetplayerid, mode);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION playerSpectateVehicle(lua_State *L)
{
	int playerid;
	int targetvehicleid;
	int mode = SPECTATE_MODE_NORMAL;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(targetvehicleid);
	argReader.ReadNumber(mode);

	bool success = PlayerSpectateVehicle(playerid, targetvehicleid, mode);
	lua_pushboolean(L, success);

	return 1;
}

LUA_FUNCTION startRecordingPlayerData(lua_State *L)
{
	int playerid;
	int recordtype;
	std::string recordname;
	
	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(recordtype);
	argReader.ReadString(recordname);

	StartRecordingPlayerData(playerid, recordtype, recordname.c_str());

	return 0;
}

LUA_FUNCTION stopRecordingPlayerData(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	StopRecordingPlayerData(playerid);

	return 0;
}

LUA_FUNCTION selectTextDraw(lua_State *L)
{
	int playerid;
	int hovercolor;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadNumber(hovercolor);

	SelectTextDraw(playerid, hovercolor);

	return 0;
}

LUA_FUNCTION cancelSelectTextDraw(lua_State *L)
{
	int playerid;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);

	CancelSelectTextDraw(playerid);

	return 0;
}

LUA_FUNCTION createExplosionForPlayer(lua_State *L)
{
	int playerid;
	float X;
	float Y;
	float Z;
	int type;
	float Radius;

	ArgReader argReader(L);
	argReader.ReadNumber(playerid);
	argReader.ReadFloat(X);
	argReader.ReadFloat(Y);
	argReader.ReadFloat(Z);
	argReader.ReadNumber(type);
	argReader.ReadFloat(Radius);

	CreateExplosionForPlayer(playerid, X, Y, Z, type, Radius);
	lua_pushboolean(L, true);

	return 1;
}