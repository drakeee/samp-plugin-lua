#include "CLuaManager.h"
#include "CGlobalContainer.h"

CLuaManager::CLuaManager(std::string rName, std::string scriptFile)
{
	resourceName = rName;
	fileName = scriptFile;
	lua_VM = NULL;
	//luaReferences.clear();
}

CLuaManager::~CLuaManager()
{
	//CUtility::printf("CLuaManager destrcutor!");
	CallInitExit("onScriptExit");
	//CContainer::LuaManager::Delete(lua_VM);
}

void CLuaManager::InitVM()
{
	lua_VM = luaL_newstate();
	luaL_openlibs(lua_VM);

	std::string fileWithPath("lua_scripts/" + resourceName + "/" + fileName);
	//CUtility::printf("Utilize: %d", lua_VM);
	int s = luaL_loadfile(lua_VM, fileWithPath.c_str());

	DisableFunctions(lua_VM);
	RegisterFunctions(lua_VM);

	CContainer::LuaManager::Add(lua_VM, this);
}

void CLuaManager::StartLua(void)
{
	int s = lua_pcall(lua_VM, 0, LUA_MULTRET, 0);

	if (s != 0)
	{
		CUtility::printf("-- %s", lua_tostring(lua_VM, -1));
		lua_pop(lua_VM, 1);
	}

	CallInitExit("onScriptInit");
}

void CLuaManager::CallInitExit(const char* scriptInit)
{
	std::map<std::string, std::vector<ReferenceStruct>> temp = CContainer::LuaReference::GetContainer();
	std::vector<ReferenceStruct> a = temp[scriptInit];
	for (std::vector<ReferenceStruct>::iterator it = a.begin(); it != a.end(); ++it)
	{
		if (it->lua_VM == lua_VM)
		{
			lua_rawgeti(lua_VM, LUA_REGISTRYINDEX, it->reference);

			if (lua_pcall(lua_VM, 0, LUA_MULTRET, 0) != 0)
			{
				CUtility::printf("Error calling LUA callback (%s)", scriptInit);
				CUtility::printf("-- %s", lua_tostring(lua_VM, -1));
				lua_pop(lua_VM, 1);
			}
		}
	}
}

void CLuaManager::DisableFunctions(lua_State *L)
{
	lua_register(L, "dofile", CLuaFunctions::DisableFunction);
	lua_register(L, "loadfile", CLuaFunctions::DisableFunction);
	lua_register(L, "loadlib", CLuaFunctions::DisableFunction);
	lua_register(L, "require", CLuaFunctions::DisableFunction);
	lua_register(L, "setfenv", CLuaFunctions::DisableFunction);
	lua_register(L, "getfenv", CLuaFunctions::DisableFunction);
}

void CLuaManager::RegisterFunctions(lua_State *L)
{
	lua_register(L, "print", CLuaFunctions::print);
	lua_register(L, "addEventHandler", CLuaFunctions::addEventHandler);
	lua_register(L, "addCommandHandler", CLuaFunctions::addCommandHandler);

	//Functions - Util
	lua_register(L, "sendClientMessage", CLuaFunctions::sendClientMessage);
	lua_register(L, "sendClientMessageToAll", CLuaFunctions::sendClientMessageToAll);
	lua_register(L, "sendPlayerMessageToPlayer", CLuaFunctions::sendPlayerMessageToPlayer);
	lua_register(L, "sendPlayerMessageToAll", CLuaFunctions::sendPlayerMessageToAll);
	lua_register(L, "sendDeathMessage", CLuaFunctions::sendDeathMessage);
	lua_register(L, "sendDeathMessageToPlayer", CLuaFunctions::sendDeathMessageToPlayer);
	lua_register(L, "gameTextForAll", CLuaFunctions::gameTextForAll);
	lua_register(L, "gameTextForPlayer", CLuaFunctions::gameTextForPlayer);
	//SetTimer(funcname[], interval, repeating);
	//SetTimerEx(funcname[], interval, repeating, const format[], { Float, _ }:...);
	//KillTimer(timerid);
	lua_register(L, "getTickCount", CLuaFunctions::getTickCount);
	lua_register(L, "getMaxPlayers", CLuaFunctions::getMaxPlayers);
	//CallRemoteFunction(const function[], const format[], { Float, _ }:...);
	//CallLocalFunction(const function[], const format[], { Float, _ }:...);
	lua_register(L, "vectorSize", CLuaFunctions::vectorSize);
	lua_register(L, "getPlayerPoolSize", CLuaFunctions::getPlayerPoolSize);
	lua_register(L, "getVehiclePoolSize", CLuaFunctions::getVehiclePoolSize);
	lua_register(L, "getActorPoolSize", CLuaFunctions::getActorPoolSize);

	//Functions - Game
	lua_register(L, "setGameModeText", CLuaFunctions::setGameModeText);
	lua_register(L, "setTeamCount", CLuaFunctions::setTeamCount);
	lua_register(L, "addPlayerClass", CLuaFunctions::addPlayerClass);
	lua_register(L, "addPlayerClassEx", CLuaFunctions::addPlayerClassEx);
	lua_register(L, "addStaticVehicle", CLuaFunctions::addStaticVehicle);
	lua_register(L, "addStaticVehicleEx", CLuaFunctions::addStaticVehicleEx);
	lua_register(L, "addStaticPickup", CLuaFunctions::addStaticPickup);
	lua_register(L, "createPickup", CLuaFunctions::createPickup);
	lua_register(L, "destroyPickup", CLuaFunctions::destroyPickup);
	lua_register(L, "showNameTags", CLuaFunctions::showNameTags);
	lua_register(L, "showPlayerMarkers", CLuaFunctions::showPlayerMarkers);
	lua_register(L, "gameModeExit", CLuaFunctions::gameModeExit);
	lua_register(L, "setWorldTime", CLuaFunctions::setWorldTime);
	lua_register(L, "getWeaponName", CLuaFunctions::getWeaponName);
	lua_register(L, "enableTirePopping", CLuaFunctions::enableTirePopping);
	lua_register(L, "enableVehicleFriendlyFire", CLuaFunctions::enableVehicleFriendlyFire);
	lua_register(L, "allowInteriorWeapons", CLuaFunctions::allowInteriorWeapons);
	lua_register(L, "setWeather", CLuaFunctions::setWeather);
	lua_register(L, "setGravity", CLuaFunctions::setGravity);
	lua_register(L, "allowAdminTeleport", CLuaFunctions::allowAdminTeleport);
	lua_register(L, "setDeathDropAmount", CLuaFunctions::setDeathDropAmount);
	lua_register(L, "createExplosion", CLuaFunctions::createExplosion);
	lua_register(L, "enableZoneNames", CLuaFunctions::enableZoneNames);
	lua_register(L, "usePlayerPedAnims", CLuaFunctions::usePlayerPedAnims);
	lua_register(L, "disableInteriorEnterExits", CLuaFunctions::disableInteriorEnterExits);
	lua_register(L, "setNameTagDrawDistance", CLuaFunctions::setNameTagDrawDistance);
	lua_register(L, "disableNameTagLOS", CLuaFunctions::disableNameTagLOS);
	lua_register(L, "limitGlobalChatRadius", CLuaFunctions::limitGlobalChatRadius);
	lua_register(L, "limitPlayerMarkerRadius", CLuaFunctions::limitPlayerMarkerRadius);

	//Functions - NPC
	lua_register(L, "connectNPC", CLuaFunctions::connectNPC);
	lua_register(L, "isPlayerNPC", CLuaFunctions::isPlayerNPC);

	//Functions - Admin
	lua_register(L, "isPlayerAdmin", CLuaFunctions::isPlayerAdmin);
	lua_register(L, "kickPlayer", CLuaFunctions::kickPlayer);
	lua_register(L, "banPlayer", CLuaFunctions::banPlayer);
	lua_register(L, "banPlayerEx", CLuaFunctions::banPlayerEx);
	lua_register(L, "sendRconCommand", CLuaFunctions::sendRconCommand);
	lua_register(L, "getServerVarAsString", CLuaFunctions::getServerVarAsString);
	lua_register(L, "getServerVarAsInt", CLuaFunctions::getServerVarAsInt);
	lua_register(L, "getServerVarAsBool", CLuaFunctions::getServerVarAsBool);
	lua_register(L, "getPlayerNetworkStats", CLuaFunctions::getPlayerNetworkStats);
	lua_register(L, "getNetworkStats", CLuaFunctions::getNetworkStats);
	lua_register(L, "getPlayerVersion", CLuaFunctions::getPlayerVersion);
	lua_register(L, "blockIpAddress", CLuaFunctions::blockIpAddress);
	lua_register(L, "unBlockIpAddress", CLuaFunctions::unBlockIpAddress);

	//Functions - Network stats
	lua_register(L, "getServerTickRate", CLuaFunctions::getServerTickRate);
	lua_register(L, "netStats_GetConnectedTime", CLuaFunctions::netStats_GetConnectedTime);
	lua_register(L, "netStats_MessagesReceived", CLuaFunctions::netStats_MessagesReceived);
	lua_register(L, "netStats_BytesReceived", CLuaFunctions::netStats_BytesReceived);
	lua_register(L, "netStats_MessagesSent", CLuaFunctions::netStats_MessagesSent);
	lua_register(L, "netStats_BytesSent", CLuaFunctions::netStats_BytesSent);
	lua_register(L, "netStats_MessagesRecvPerSecond", CLuaFunctions::netStats_MessagesRecvPerSecond);
	lua_register(L, "netStats_PacketLossPercent", CLuaFunctions::netStats_PacketLossPercent);
	lua_register(L, "netStats_ConnectionStatus", CLuaFunctions::netStats_ConnectionStatus);
	lua_register(L, "netStats_GetIpPort", CLuaFunctions::netStats_GetIpPort);

	//Functions - Menu
	lua_register(L, "createMenu", CLuaFunctions::createMenu);
	lua_register(L, "destroyMenu", CLuaFunctions::destroyMenu);
	lua_register(L, "addMenuItem", CLuaFunctions::addMenuItem);
	lua_register(L, "setMenuColumnHeader", CLuaFunctions::setMenuColumnHeader);
	lua_register(L, "showMenuForPlayer", CLuaFunctions::showMenuForPlayer);
	lua_register(L, "hideMenuForPlayer", CLuaFunctions::hideMenuForPlayer);
	lua_register(L, "isValidMenu", CLuaFunctions::isValidMenu);
	lua_register(L, "disableMenu", CLuaFunctions::disableMenu);
	lua_register(L, "disableMenuRow", CLuaFunctions::disableMenuRow);
	lua_register(L, "getPlayerMenu", CLuaFunctions::getPlayerMenu);

	//Functions - TextDraw
	lua_register(L, "textDrawCreate", CLuaFunctions::textDrawCreate);
	lua_register(L, "textDrawDestroy", CLuaFunctions::textDrawDestroy);
	lua_register(L, "textDrawLetterSize", CLuaFunctions::textDrawLetterSize);
	lua_register(L, "textDrawTextSize", CLuaFunctions::textDrawTextSize);
	lua_register(L, "textDrawAlignment", CLuaFunctions::textDrawAlignment);
	lua_register(L, "textDrawColor", CLuaFunctions::textDrawColor);
	lua_register(L, "textDrawUseBox", CLuaFunctions::textDrawUseBox);
	lua_register(L, "textDrawBoxColor", CLuaFunctions::textDrawBoxColor);
	lua_register(L, "textDrawSetShadow", CLuaFunctions::textDrawSetShadow);
	lua_register(L, "textDrawSetOutline", CLuaFunctions::textDrawSetOutline);
	lua_register(L, "textDrawBackgroundColor", CLuaFunctions::textDrawBackgroundColor);
	lua_register(L, "textDrawFont", CLuaFunctions::textDrawFont);
	lua_register(L, "textDrawSetProportional", CLuaFunctions::textDrawSetProportional);
	lua_register(L, "textDrawSetSelectable", CLuaFunctions::textDrawSetSelectable);
	lua_register(L, "textDrawShowForPlayer", CLuaFunctions::textDrawShowForPlayer);
	lua_register(L, "textDrawHideForPlayer", CLuaFunctions::textDrawHideForPlayer);
	lua_register(L, "textDrawShowForAll", CLuaFunctions::textDrawShowForAll);
	lua_register(L, "textDrawHideForAll", CLuaFunctions::textDrawHideForAll);
	lua_register(L, "textDrawSetString", CLuaFunctions::textDrawSetString);
	lua_register(L, "textDrawSetPreviewModel", CLuaFunctions::textDrawSetPreviewModel);
	lua_register(L, "textDrawSetPreviewRot", CLuaFunctions::textDrawSetPreviewRot);
	lua_register(L, "textDrawSetPreviewVehCol", CLuaFunctions::textDrawSetPreviewVehCol);

	//Functions - GangZone
	lua_register(L, "gangZoneCreate", CLuaFunctions::gangZoneCreate);
	lua_register(L, "gangZoneDestroy", CLuaFunctions::gangZoneDestroy);
	lua_register(L, "gangZoneShowForPlayer", CLuaFunctions::gangZoneShowForPlayer);
	lua_register(L, "gangZoneShowForAll", CLuaFunctions::gangZoneShowForAll);
	lua_register(L, "gangZoneHideForPlayer", CLuaFunctions::gangZoneHideForPlayer);
	lua_register(L, "gangZoneHideForAll", CLuaFunctions::gangZoneHideForAll);
	lua_register(L, "gangZoneFlashForPlayer", CLuaFunctions::gangZoneFlashForPlayer);
	lua_register(L, "gangZoneFlashForAll", CLuaFunctions::gangZoneFlashForAll);
	lua_register(L, "gangZoneStopFlashForPlayer", CLuaFunctions::gangZoneStopFlashForPlayer);
	lua_register(L, "gangZoneStopFlashForAll", CLuaFunctions::gangZoneStopFlashForAll);

	//Functions - TextLabel
	lua_register(L, "create3DTextLabel", CLuaFunctions::create3DTextLabel);
	lua_register(L, "delete3DTextLabel", CLuaFunctions::delete3DTextLabel);
	lua_register(L, "attach3DTextLabelToPlayer", CLuaFunctions::attach3DTextLabelToPlayer);
	lua_register(L, "attach3DTextLabelToVehicle", CLuaFunctions::attach3DTextLabelToVehicle);
	lua_register(L, "update3DTextLabelText", CLuaFunctions::update3DTextLabelText);

	//Functions - Per-player TextLabel
	lua_register(L, "createPlayer3DTextLabel", CLuaFunctions::createPlayer3DTextLabel);
	lua_register(L, "deletePlayer3DTextLabel", CLuaFunctions::deletePlayer3DTextLabel);
	lua_register(L, "updatePlayer3DTextLabelText", CLuaFunctions::updatePlayer3DTextLabelText);

	//Functions - Dialog
	lua_register(L, "showPlayerDialog", CLuaFunctions::showPlayerDialog);

	//Functions - HTTP
	lua_register(L, "lua_HTTP", CLuaFunctions::lua_HTTP);

	//Functions - Actor
	lua_register(L, "createActor", CLuaFunctions::createActor);
	lua_register(L, "destroyActor", CLuaFunctions::destroyActor);
	lua_register(L, "isActorStreamedIn", CLuaFunctions::isActorStreamedIn);
	lua_register(L, "setActorVirtualWorld", CLuaFunctions::setActorVirtualWorld);
	lua_register(L, "getActorVirtualWorld", CLuaFunctions::getActorVirtualWorld);
	lua_register(L, "applyActorAnimation", CLuaFunctions::applyActorAnimation);
	lua_register(L, "clearActorAnimations", CLuaFunctions::clearActorAnimations);
	lua_register(L, "setActorPos", CLuaFunctions::setActorPos);
	lua_register(L, "getActorPos", CLuaFunctions::getActorPos);
	lua_register(L, "setActorFacingAngle", CLuaFunctions::setActorFacingAngle);
	lua_register(L, "getActorFacingAngle", CLuaFunctions::getActorFacingAngle);
	lua_register(L, "setActorHealth", CLuaFunctions::setActorHealth);
	lua_register(L, "getActorHealth", CLuaFunctions::getActorHealth);
	lua_register(L, "setActorInvulnerable", CLuaFunctions::setActorInvulnerable);
	lua_register(L, "isActorInvulnerable", CLuaFunctions::isActorInvulnerable);
	lua_register(L, "isValidActor", CLuaFunctions::isValidActor);

	//Functions - Object
	lua_register(L, "createObject", CLuaFunctions::createObject);
	lua_register(L, "attachObjectToVehicle", CLuaFunctions::attachObjectToVehicle);
	lua_register(L, "attachObjectToObject", CLuaFunctions::attachObjectToObject);
	lua_register(L, "attachObjectToPlayer", CLuaFunctions::attachObjectToPlayer);
	lua_register(L, "setObjectPos", CLuaFunctions::setObjectPos);
	lua_register(L, "getObjectPos", CLuaFunctions::getObjectPos);
	lua_register(L, "setObjectRot", CLuaFunctions::setObjectRot);
	lua_register(L, "getObjectRot", CLuaFunctions::getObjectRot);
	lua_register(L, "getObjectModel", CLuaFunctions::getObjectModel);
	lua_register(L, "setObjectNoCameraCol", CLuaFunctions::setObjectNoCameraCol);
	lua_register(L, "isValidObject", CLuaFunctions::isValidObject);
	lua_register(L, "destroyObject", CLuaFunctions::destroyObject);
	lua_register(L, "moveObject", CLuaFunctions::moveObject);
	lua_register(L, "stopObject", CLuaFunctions::stopObject);
	lua_register(L, "isObjectMoving", CLuaFunctions::isObjectMoving);
	lua_register(L, "editObject", CLuaFunctions::editObject);
	lua_register(L, "editPlayerObject", CLuaFunctions::editPlayerObject);
	lua_register(L, "selectObject", CLuaFunctions::selectObject);
	lua_register(L, "cancelEdit", CLuaFunctions::cancelEdit);
	lua_register(L, "createPlayerObject", CLuaFunctions::createPlayerObject);
	lua_register(L, "attachPlayerObjectToVehicle", CLuaFunctions::attachPlayerObjectToVehicle);
	lua_register(L, "setPlayerObjectPos", CLuaFunctions::setPlayerObjectPos);
	lua_register(L, "getPlayerObjectPos", CLuaFunctions::getPlayerObjectPos);
	lua_register(L, "setPlayerObjectRot", CLuaFunctions::setPlayerObjectRot);
	lua_register(L, "getPlayerObjectRot", CLuaFunctions::getPlayerObjectRot);
	lua_register(L, "getPlayerObjectModel", CLuaFunctions::getPlayerObjectModel);
	lua_register(L, "setPlayerObjectNoCameraCol", CLuaFunctions::setPlayerObjectNoCameraCol);
	lua_register(L, "isValidPlayerObject", CLuaFunctions::isValidPlayerObject);
	lua_register(L, "destroyPlayerObject", CLuaFunctions::destroyPlayerObject);
	lua_register(L, "movePlayerObject", CLuaFunctions::movePlayerObject);
	lua_register(L, "stopPlayerObject", CLuaFunctions::stopPlayerObject);
	lua_register(L, "isPlayerObjectMoving", CLuaFunctions::isPlayerObjectMoving);
	lua_register(L, "attachPlayerObjectToPlayer", CLuaFunctions::attachPlayerObjectToPlayer);
	lua_register(L, "setObjectMaterial", CLuaFunctions::setObjectMaterial);
	lua_register(L, "setPlayerObjectMaterial", CLuaFunctions::setPlayerObjectMaterial);
	lua_register(L, "setObjectMaterialText", CLuaFunctions::setObjectMaterialText);
	lua_register(L, "setPlayerObjectMaterialText", CLuaFunctions::setPlayerObjectMaterialText);
	lua_register(L, "setObjectsDefaultCameraCol", CLuaFunctions::setObjectsDefaultCameraCol);

	//Functions - Vehicle
	lua_register(L, "createVehicle", CLuaFunctions::createVehicle);
	lua_register(L, "destroyVehicle", CLuaFunctions::destroyVehicle);
	lua_register(L, "isVehicleStreamedIn", CLuaFunctions::isVehicleStreamedIn);
	lua_register(L, "getVehiclePos", CLuaFunctions::getVehiclePos);
	lua_register(L, "setVehiclePos", CLuaFunctions::setVehiclePos);
	lua_register(L, "getVehicleZAngle", CLuaFunctions::getVehicleZAngle);
	lua_register(L, "getVehicleRotationQuat", CLuaFunctions::getVehicleRotationQuat);
	lua_register(L, "getVehicleDistanceFromPoint", CLuaFunctions::getVehicleDistanceFromPoint);
	lua_register(L, "setVehicleZAngle", CLuaFunctions::setVehicleZAngle);
	lua_register(L, "setVehicleParamsForPlayer", CLuaFunctions::setVehicleParamsForPlayer);
	lua_register(L, "manualVehicleEngineAndLights", CLuaFunctions::manualVehicleEngineAndLights);
	lua_register(L, "setVehicleParamsEx", CLuaFunctions::setVehicleParamsEx);
	lua_register(L, "getVehicleParamsEx", CLuaFunctions::getVehicleParamsEx);
	lua_register(L, "getVehicleParamsSirenState", CLuaFunctions::getVehicleParamsSirenState);
	lua_register(L, "setVehicleParamsCarDoors", CLuaFunctions::setVehicleParamsCarDoors);
	lua_register(L, "getVehicleParamsCarDoors", CLuaFunctions::getVehicleParamsCarDoors);
	lua_register(L, "setVehicleParamsCarWindows", CLuaFunctions::setVehicleParamsCarWindows);
	lua_register(L, "getVehicleParamsCarWindows", CLuaFunctions::getVehicleParamsCarWindows);
	lua_register(L, "setVehicleToRespawn", CLuaFunctions::setVehicleToRespawn);
	lua_register(L, "linkVehicleToInterior", CLuaFunctions::linkVehicleToInterior);
	lua_register(L, "addVehicleComponent", CLuaFunctions::addVehicleComponent);
	lua_register(L, "removeVehicleComponent", CLuaFunctions::removeVehicleComponent);
	lua_register(L, "changeVehicleColor", CLuaFunctions::changeVehicleColor);
	lua_register(L, "changeVehiclePaintjob", CLuaFunctions::changeVehiclePaintjob);
	lua_register(L, "setVehicleHealth", CLuaFunctions::setVehicleHealth);
	lua_register(L, "getVehicleHealth", CLuaFunctions::getVehicleHealth);
	lua_register(L, "attachTrailerToVehicle", CLuaFunctions::attachTrailerToVehicle);
	lua_register(L, "detachTrailerFromVehicle", CLuaFunctions::detachTrailerFromVehicle);
	lua_register(L, "isTrailerAttachedToVehicle", CLuaFunctions::isTrailerAttachedToVehicle);
	lua_register(L, "getVehicleTrailer", CLuaFunctions::getVehicleTrailer);
	lua_register(L, "setVehicleNumberPlate", CLuaFunctions::setVehicleNumberPlate);
	lua_register(L, "getVehicleModel", CLuaFunctions::getVehicleModel);
	lua_register(L, "getVehicleComponentInSlot", CLuaFunctions::getVehicleComponentInSlot);
	lua_register(L, "getVehicleComponentType", CLuaFunctions::getVehicleComponentType);
	lua_register(L, "repairVehicle", CLuaFunctions::repairVehicle);
	lua_register(L, "getVehicleVelocity", CLuaFunctions::getVehicleVelocity);
	lua_register(L, "setVehicleVelocity", CLuaFunctions::setVehicleVelocity);
	lua_register(L, "setVehicleAngularVelocity", CLuaFunctions::setVehicleAngularVelocity);
	lua_register(L, "getVehicleDamageStatus", CLuaFunctions::getVehicleDamageStatus);
	lua_register(L, "updateVehicleDamageStatus", CLuaFunctions::updateVehicleDamageStatus);
	lua_register(L, "getVehicleModelInfo", CLuaFunctions::getVehicleModelInfo);
	lua_register(L, "setVehicleVirtualWorld", CLuaFunctions::setVehicleVirtualWorld);
	lua_register(L, "getVehicleVirtualWorld", CLuaFunctions::getVehicleVirtualWorld);

	//Functions - Players
	lua_register(L, "setSpawnInfo", CLuaFunctions::setSpawnInfo);
	lua_register(L, "spawnPlayer", CLuaFunctions::spawnPlayer);
	lua_register(L, "setPlayerPos", CLuaFunctions::setPlayerPos);
	lua_register(L, "setPlayerPosFindZ", CLuaFunctions::setPlayerPosFindZ);
	lua_register(L, "getPlayerPos", CLuaFunctions::getPlayerPos);
	lua_register(L, "setPlayerFacingAngle", CLuaFunctions::setPlayerFacingAngle);
	lua_register(L, "getPlayerFacingAngle", CLuaFunctions::getPlayerFacingAngle);
	lua_register(L, "isPlayerInRangeOfPoint", CLuaFunctions::isPlayerInRangeOfPoint);
	lua_register(L, "getPlayerDistanceFromPoint", CLuaFunctions::getPlayerDistanceFromPoint);
	lua_register(L, "isPlayerStreamedIn", CLuaFunctions::isPlayerStreamedIn);
	lua_register(L, "setPlayerInterior", CLuaFunctions::setPlayerInterior);
	lua_register(L, "getPlayerInterior", CLuaFunctions::getPlayerInterior);
	lua_register(L, "setPlayerHealth", CLuaFunctions::setPlayerHealth);
	lua_register(L, "getPlayerHealth", CLuaFunctions::getPlayerHealth);
	lua_register(L, "setPlayerArmour", CLuaFunctions::setPlayerArmour);
	lua_register(L, "getPlayerArmour", CLuaFunctions::getPlayerArmour);
	lua_register(L, "setPlayerAmmo", CLuaFunctions::setPlayerAmmo);
	lua_register(L, "getPlayerAmmo", CLuaFunctions::getPlayerAmmo);
	lua_register(L, "getPlayerWeaponState", CLuaFunctions::getPlayerWeaponState);
	lua_register(L, "getPlayerTargetPlayer", CLuaFunctions::getPlayerTargetPlayer);
	lua_register(L, "getPlayerTargetActor", CLuaFunctions::getPlayerTargetActor);
	lua_register(L, "setPlayerTeam", CLuaFunctions::setPlayerTeam);
	lua_register(L, "getPlayerTeam", CLuaFunctions::getPlayerTeam);
	lua_register(L, "setPlayerScore", CLuaFunctions::setPlayerScore);
	lua_register(L, "getPlayerScore", CLuaFunctions::getPlayerScore);
	lua_register(L, "getPlayerDrunkLevel", CLuaFunctions::getPlayerDrunkLevel);
	lua_register(L, "setPlayerDrunkLevel", CLuaFunctions::setPlayerDrunkLevel);
	lua_register(L, "setPlayerColor", CLuaFunctions::setPlayerColor);
	lua_register(L, "getPlayerColor", CLuaFunctions::getPlayerColor);
	lua_register(L, "setPlayerSkin", CLuaFunctions::setPlayerSkin);
	lua_register(L, "getPlayerSkin", CLuaFunctions::getPlayerSkin);
	lua_register(L, "givePlayerWeapon", CLuaFunctions::givePlayerWeapon);
	lua_register(L, "resetPlayerWeapons", CLuaFunctions::resetPlayerWeapons);
	lua_register(L, "setPlayerArmedWeapon", CLuaFunctions::setPlayerArmedWeapon);
	lua_register(L, "getPlayerWeaponData", CLuaFunctions::getPlayerWeaponData);
	lua_register(L, "givePlayerMoney", CLuaFunctions::givePlayerMoney);
	lua_register(L, "resetPlayerMoney", CLuaFunctions::resetPlayerMoney);
	lua_register(L, "setPlayerName", CLuaFunctions::setPlayerName);
	lua_register(L, "getPlayerMoney", CLuaFunctions::getPlayerMoney);
	lua_register(L, "getPlayerState", CLuaFunctions::getPlayerState);
	lua_register(L, "getPlayerIp", CLuaFunctions::getPlayerIp);
	lua_register(L, "getPlayerPing", CLuaFunctions::getPlayerPing);
	lua_register(L, "getPlayerWeapon", CLuaFunctions::getPlayerWeapon);
	lua_register(L, "getPlayerKeys", CLuaFunctions::getPlayerKeys);
	lua_register(L, "getPlayerName", CLuaFunctions::getPlayerName);
	lua_register(L, "setPlayerTime", CLuaFunctions::setPlayerTime);
	lua_register(L, "getPlayerTime", CLuaFunctions::getPlayerTime);
	lua_register(L, "togglePlayerClock", CLuaFunctions::togglePlayerClock);
	lua_register(L, "setPlayerWeather", CLuaFunctions::setPlayerWeather);
	lua_register(L, "forceClassSelection", CLuaFunctions::forceClassSelection);
	lua_register(L, "setPlayerWantedLevel", CLuaFunctions::setPlayerWantedLevel);
	lua_register(L, "getPlayerWantedLevel", CLuaFunctions::getPlayerWantedLevel);
	lua_register(L, "setPlayerFightingStyle", CLuaFunctions::setPlayerFightingStyle);
	lua_register(L, "getPlayerFightingStyle", CLuaFunctions::getPlayerFightingStyle);
	lua_register(L, "setPlayerVelocity", CLuaFunctions::setPlayerVelocity);
	lua_register(L, "getPlayerVelocity", CLuaFunctions::getPlayerVelocity);
	lua_register(L, "playCrimeReportForPlayer", CLuaFunctions::playCrimeReportForPlayer);
	lua_register(L, "playAudioStreamForPlayer", CLuaFunctions::playAudioStreamForPlayer);
	lua_register(L, "stopAudioStreamForPlayer", CLuaFunctions::stopAudioStreamForPlayer);
	lua_register(L, "setPlayerShopName", CLuaFunctions::setPlayerShopName);
	lua_register(L, "setPlayerSkillLevel", CLuaFunctions::setPlayerSkillLevel);
	lua_register(L, "getPlayerSurfingVehicleID", CLuaFunctions::getPlayerSurfingVehicleID);
	lua_register(L, "getPlayerSurfingObjectID", CLuaFunctions::getPlayerSurfingObjectID);
	lua_register(L, "removeBuildingForPlayer", CLuaFunctions::removeBuildingForPlayer);
	lua_register(L, "getPlayerLastShotVectors", CLuaFunctions::getPlayerLastShotVectors);
	lua_register(L, "setPlayerAttachedObject", CLuaFunctions::setPlayerAttachedObject);
	lua_register(L, "removePlayerAttachedObject", CLuaFunctions::removePlayerAttachedObject);
	lua_register(L, "isPlayerAttachedObjectSlotUsed", CLuaFunctions::isPlayerAttachedObjectSlotUsed);
	lua_register(L, "editAttachedObject", CLuaFunctions::editAttachedObject);
	lua_register(L, "createPlayerTextDraw", CLuaFunctions::createPlayerTextDraw);
	lua_register(L, "playerTextDrawDestroy", CLuaFunctions::playerTextDrawDestroy);
	lua_register(L, "playerTextDrawLetterSize", CLuaFunctions::playerTextDrawLetterSize);
	lua_register(L, "playerTextDrawTextSize", CLuaFunctions::playerTextDrawTextSize);
	lua_register(L, "playerTextDrawAlignment", CLuaFunctions::playerTextDrawAlignment);
	lua_register(L, "playerTextDrawColor", CLuaFunctions::playerTextDrawColor);
	lua_register(L, "playerTextDrawUseBox", CLuaFunctions::playerTextDrawUseBox);
	lua_register(L, "playerTextDrawBoxColor", CLuaFunctions::playerTextDrawBoxColor);
	lua_register(L, "playerTextDrawSetShadow", CLuaFunctions::playerTextDrawSetShadow);
	lua_register(L, "playerTextDrawSetOutline", CLuaFunctions::playerTextDrawSetOutline);
	lua_register(L, "playerTextDrawBackgroundColor", CLuaFunctions::playerTextDrawBackgroundColor);
	lua_register(L, "playerTextDrawFont", CLuaFunctions::playerTextDrawFont);
	lua_register(L, "playerTextDrawSetProportional", CLuaFunctions::playerTextDrawSetProportional);
	lua_register(L, "playerTextDrawSetSelectable", CLuaFunctions::playerTextDrawSetSelectable);
	lua_register(L, "playerTextDrawShow", CLuaFunctions::playerTextDrawShow);
	lua_register(L, "playerTextDrawHide", CLuaFunctions::playerTextDrawHide);
	lua_register(L, "playerTextDrawSetString", CLuaFunctions::playerTextDrawSetString);
	lua_register(L, "playerTextDrawSetPreviewModel", CLuaFunctions::playerTextDrawSetPreviewModel);
	lua_register(L, "playerTextDrawSetPreviewRot", CLuaFunctions::playerTextDrawSetPreviewRot);
	lua_register(L, "playerTextDrawSetPreviewVehCol", CLuaFunctions::playerTextDrawSetPreviewVehCol);
	lua_register(L, "setPVarInt", CLuaFunctions::setPVarInt);
	lua_register(L, "getPVarInt", CLuaFunctions::getPVarInt);
	lua_register(L, "setPVarString", CLuaFunctions::setPVarString);
	lua_register(L, "getPVarString", CLuaFunctions::getPVarString);
	lua_register(L, "setPVarFloat", CLuaFunctions::setPVarFloat);
	lua_register(L, "getPVarFloat", CLuaFunctions::getPVarFloat);
	lua_register(L, "deletePVar", CLuaFunctions::deletePVar);
	lua_register(L, "getPVarsUpperIndex", CLuaFunctions::getPVarsUpperIndex);
	lua_register(L, "getPVarNameAtIndex", CLuaFunctions::getPVarNameAtIndex);
	lua_register(L, "getPVarType", CLuaFunctions::getPVarType);
	lua_register(L, "setPlayerChatBubble", CLuaFunctions::setPlayerChatBubble);
	lua_register(L, "putPlayerInVehicle", CLuaFunctions::putPlayerInVehicle);
	lua_register(L, "getPlayerVehicleID", CLuaFunctions::getPlayerVehicleID);
	lua_register(L, "getPlayerVehicleSeat", CLuaFunctions::getPlayerVehicleSeat);
	lua_register(L, "removePlayerFromVehicle", CLuaFunctions::removePlayerFromVehicle);
	lua_register(L, "togglePlayerControllable", CLuaFunctions::togglePlayerControllable);
	lua_register(L, "playerPlaySound", CLuaFunctions::playerPlaySound);
	lua_register(L, "applyAnimation", CLuaFunctions::applyAnimation);
	lua_register(L, "clearAnimations", CLuaFunctions::clearAnimations);
	lua_register(L, "getPlayerAnimationIndex", CLuaFunctions::getPlayerAnimationIndex);
	lua_register(L, "getAnimationName", CLuaFunctions::getAnimationName);
	lua_register(L, "getPlayerSpecialAction", CLuaFunctions::getPlayerSpecialAction);
	lua_register(L, "setPlayerSpecialAction", CLuaFunctions::setPlayerSpecialAction);
	lua_register(L, "disableRemoteVehicleCollisions", CLuaFunctions::disableRemoteVehicleCollisions);
	lua_register(L, "setPlayerCheckpoint", CLuaFunctions::setPlayerCheckpoint);
	lua_register(L, "disablePlayerCheckpoint", CLuaFunctions::disablePlayerCheckpoint);
	lua_register(L, "setPlayerRaceCheckpoint", CLuaFunctions::setPlayerRaceCheckpoint);
	lua_register(L, "disablePlayerRaceCheckpoint", CLuaFunctions::disablePlayerRaceCheckpoint);
	lua_register(L, "setPlayerWorldBounds", CLuaFunctions::setPlayerWorldBounds);
	lua_register(L, "setPlayerMarkerForPlayer", CLuaFunctions::setPlayerMarkerForPlayer);
	lua_register(L, "showPlayerNameTagForPlayer", CLuaFunctions::showPlayerNameTagForPlayer);
	lua_register(L, "setPlayerMapIcon", CLuaFunctions::setPlayerMapIcon);
	lua_register(L, "removePlayerMapIcon", CLuaFunctions::removePlayerMapIcon);
	lua_register(L, "allowPlayerTeleport", CLuaFunctions::allowPlayerTeleport);
	lua_register(L, "setPlayerCameraPos", CLuaFunctions::setPlayerCameraPos);
	lua_register(L, "setPlayerCameraLookAt", CLuaFunctions::setPlayerCameraLookAt);
	lua_register(L, "setCameraBehindPlayer", CLuaFunctions::setCameraBehindPlayer);
	lua_register(L, "getPlayerCameraPos", CLuaFunctions::getPlayerCameraPos);
	lua_register(L, "getPlayerCameraFrontVector", CLuaFunctions::getPlayerCameraFrontVector);
	lua_register(L, "getPlayerCameraMode", CLuaFunctions::getPlayerCameraMode);
	lua_register(L, "enablePlayerCameraTarget", CLuaFunctions::enablePlayerCameraTarget);
	lua_register(L, "getPlayerCameraTargetObject", CLuaFunctions::getPlayerCameraTargetObject);
	lua_register(L, "getPlayerCameraTargetVehicle", CLuaFunctions::getPlayerCameraTargetVehicle);
	lua_register(L, "getPlayerCameraTargetPlayer", CLuaFunctions::getPlayerCameraTargetPlayer);
	lua_register(L, "getPlayerCameraTargetActor", CLuaFunctions::getPlayerCameraTargetActor);
	lua_register(L, "getPlayerCameraAspectRatio", CLuaFunctions::getPlayerCameraAspectRatio);
	lua_register(L, "getPlayerCameraZoom", CLuaFunctions::getPlayerCameraZoom);
	lua_register(L, "attachCameraToObject", CLuaFunctions::attachCameraToObject);
	lua_register(L, "attachCameraToPlayerObject", CLuaFunctions::attachCameraToPlayerObject);
	lua_register(L, "interpolateCameraPos", CLuaFunctions::interpolateCameraPos);
	lua_register(L, "interpolateCameraLookAt", CLuaFunctions::interpolateCameraLookAt);
	lua_register(L, "isPlayerConnected", CLuaFunctions::isPlayerConnected);
	lua_register(L, "isPlayerInVehicle", CLuaFunctions::isPlayerInVehicle);
	lua_register(L, "isPlayerInAnyVehicle", CLuaFunctions::isPlayerInAnyVehicle);
	lua_register(L, "isPlayerInCheckpoint", CLuaFunctions::isPlayerInCheckpoint);
	lua_register(L, "isPlayerInRaceCheckpoint", CLuaFunctions::isPlayerInRaceCheckpoint);
	lua_register(L, "setPlayerVirtualWorld", CLuaFunctions::setPlayerVirtualWorld);
	lua_register(L, "getPlayerVirtualWorld", CLuaFunctions::getPlayerVirtualWorld);
	lua_register(L, "enableStuntBonusForPlayer", CLuaFunctions::enableStuntBonusForPlayer);
	lua_register(L, "enableStuntBonusForAll", CLuaFunctions::enableStuntBonusForAll);
	lua_register(L, "togglePlayerSpectating", CLuaFunctions::togglePlayerSpectating);
	lua_register(L, "playerSpectatePlayer", CLuaFunctions::playerSpectatePlayer);
	lua_register(L, "playerSpectateVehicle", CLuaFunctions::playerSpectateVehicle);
	lua_register(L, "startRecordingPlayerData", CLuaFunctions::startRecordingPlayerData);
	lua_register(L, "stopRecordingPlayerData", CLuaFunctions::stopRecordingPlayerData);
	lua_register(L, "selectTextDraw", CLuaFunctions::selectTextDraw);
	lua_register(L, "cancelSelectTextDraw", CLuaFunctions::cancelSelectTextDraw);
	lua_register(L, "createExplosionForPlayer", CLuaFunctions::createExplosionForPlayer);
}