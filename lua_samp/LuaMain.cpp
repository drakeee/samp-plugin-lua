#include "LuaMain.h"
#include "CTimer.h"

extern void *pAMXFunctions;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return sampgdk::Supports() | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	bool load = sampgdk::Load(ppData);

	s_Core = new CCore();

	//s_Core->RegisterCallback("onPlayerConnect", { ArgumentType::TYPE_INT });
	//s_Core->RegisterCallback("onGameModeInit", { ArgumentType::TYPE_NONE });
	//s_Core->RegisterCallback("onGameModeExit", { ArgumentType::TYPE_NONE });
	//s_Core->RegisterCallback("onFilterScriptInit", { ArgumentType::TYPE_NONE });
	//s_Core->RegisterCallback("onFilterScriptExit", { ArgumentType::TYPE_NONE });
	s_Core->RegisterCallback("onPlayerConnect", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerDisconnect", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerSpawn", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerDeath", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onVehicleSpawn", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onVehicleDeath", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerText", { ArgumentType::TYPE_INT, ArgumentType::TYPE_STRING });
	s_Core->RegisterCallback("onPlayerCommandText", { ArgumentType::TYPE_INT, ArgumentType::TYPE_STRING });
	s_Core->RegisterCallback("onPlayerRequestClass", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerEnterVehicle", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerExitVehicle", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerStateChange", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerEnterCheckpoint", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerLeaveCheckpoint", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerEnterRaceCheckpoint", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerLeaveRaceCheckpoint", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onRconCommand", { ArgumentType::TYPE_STRING });
	s_Core->RegisterCallback("onPlayerRequestSpawn", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onObjectMoved", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerObjectMoved", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerPickUpPickup", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onVehicleMod", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onEnterExitModShop", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onVehiclePaintjob", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onVehicleRespray", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onVehicleDamageStatusUpdate", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onUnoccupiedVehicleUpdate", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT });
	s_Core->RegisterCallback("onPlayerSelectedMenuRow", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerExitedMenu", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerInteriorChange", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerKeyStateChange", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onRconLoginAttempt", { ArgumentType::TYPE_STRING, ArgumentType::TYPE_STRING, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerUpdate", { ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerStreamIn", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerStreamOut", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onVehicleStreamIn", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onVehicleStreamOut", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onActorStreamIn", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onActorStreamOut", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onDialogResponse", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_STRING });
	s_Core->RegisterCallback("onPlayerTakeDamage", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerGiveDamage", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerGiveDamageActor", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerClickMap", { ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT });
	s_Core->RegisterCallback("onPlayerClickTextDraw", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerClickPlayerTextDraw", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onIncomingConnection", { ArgumentType::TYPE_INT, ArgumentType::TYPE_STRING, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onTrailerUpdate", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onVehicleSirenStateChange", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerClickPlayer", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT });
	s_Core->RegisterCallback("onPlayerEditObject", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT });
	s_Core->RegisterCallback("onPlayerEditAttachedObject", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT });
	s_Core->RegisterCallback("onPlayerSelectObject", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT });
	s_Core->RegisterCallback("onPlayerWeaponShot", { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT });
	
	sampgdk::logprintf(" Lua in SA-MP %s by DrAkE successfully loaded.", PLUGIN_VERSION);

	return load;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	sampgdk::logprintf(" Lua in SA-MP %s by DrAkE successfully unloaded.", PLUGIN_VERSION);
	sampgdk::Unload();
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	auto *tempContainer = s_Core->GetModuleContainer();
	for (auto it = (*tempContainer).begin(); it != (*tempContainer).end(); ++it)
	{
		(*it)->_DoPulse();
	}

	CTimer::ProcessTick();
	sampgdk::ProcessTick();
}