#include "LuaMain.h"
#include "CGlobalContainer.h"

typedef void(*logprintf_t)(char* format, ...);

logprintf_t logprintf;
extern void *pAMXFunctions;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return sampgdk::Supports() | SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	bool load = sampgdk::Load(ppData);

	//CContainer::CallbackArgsType::callbackArgumentsType["onGameModeInit"] = { ArgumentType::TYPE_NONE };
	//CContainer::CallbackArgsType::callbackArgumentsType["onGameModeExit"] = { ArgumentType::TYPE_NONE };
	//CContainer::CallbackArgsType::callbackArgumentsType["onFilterScriptInit"] = { ArgumentType::TYPE_NONE };
	//CContainer::CallbackArgsType::callbackArgumentsType["onFilterScriptExit"] = { ArgumentType::TYPE_NONE };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerConnect"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerDisconnect"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerSpawn"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerDeath"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onVehicleSpawn"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onVehicleDeath"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerText"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_STRING };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerCommandText"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_STRING };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerRequestClass"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerEnterVehicle"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerExitVehicle"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerStateChange"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerEnterCheckpoint"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerLeaveCheckpoint"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerEnterRaceCheckpoint"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerLeaveRaceCheckpoint"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onRconCommand"] = { ArgumentType::TYPE_STRING };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerRequestSpawn"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onObjectMoved"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerObjectMoved"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerPickUpPickup"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onVehicleMod"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onEnterExitModShop"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onVehiclePaintjob"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onVehicleRespray"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onVehicleDamageStatusUpdate"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onUnoccupiedVehicleUpdate"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerSelectedMenuRow"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerExitedMenu"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerInteriorChange"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerKeyStateChange"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onRconLoginAttempt"] = { ArgumentType::TYPE_STRING, ArgumentType::TYPE_STRING, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerUpdate"] = { ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerStreamIn"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerStreamOut"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onVehicleStreamIn"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onVehicleStreamOut"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onActorStreamIn"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onActorStreamOut"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onDialogResponse"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_STRING };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerTakeDamage"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerGiveDamage"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerGiveDamageActor"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerClickMap"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerClickTextDraw"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerClickPlayerTextDraw"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onIncomingConnection"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_STRING, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onTrailerUpdate"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onVehicleSirenStateChange"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerClickPlayer"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerEditObject"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerEditAttachedObject"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerSelectObject"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT };
	CContainer::CallbackArgsType::callbackArgumentsType["onPlayerWeaponShot"] = { ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_INT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT, ArgumentType::TYPE_FLOAT };

	sampgdk::logprintf(" Lua in SA-MP %s by DrAkE successfully loaded.", PLUGIN_VERSION);
	return load;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	sampgdk::logprintf(" Lua in SA-MP %s by DrAkE successfully unloaded.", PLUGIN_VERSION);
	sampgdk::Unload();
}

AMX_NATIVE_INFO PluginNatives[] =
{
	{ 0, 0 }
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	return amx_Register(amx, PluginNatives, -1);
}


PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	sampgdk::ProcessTick();
}