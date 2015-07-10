#ifndef _LUA_FUNCTIONS_HEADER_
#define _LUA_FUNCTIONS_HEADER_

#include <string>
#include "CArgumentReader.h"
#include "CLuaArguments.h"
extern "C"
{
#include "Lua_351/lua.h"
#include "Lua_351/lauxlib.h"
}

#define LUA_FUNCTION int CLuaFunctions::

namespace CLuaFunctions
{
	//Functions - Disable Function
	int DisableFunction(lua_State *L);

	//Functions - Call function from another LuaVM
	int isResourceRunning(lua_State *L);
	int call(lua_State *L);

	//Functions - Util
	int print(lua_State *L);
	int addEventHandler(lua_State *L);
	int addCommandHandler(lua_State *L);
	int setTimer(lua_State *L);
	int killTimer(lua_State *L);

	//Functions - Util
	int sendClientMessage(lua_State *L);
	int sendClientMessageToAll(lua_State *L);
	int sendPlayerMessageToPlayer(lua_State *L);
	int sendPlayerMessageToAll(lua_State *L);
	int sendDeathMessage(lua_State *L);
	int sendDeathMessageToPlayer(lua_State *L);
	int gameTextForAll(lua_State *L);
	int gameTextForPlayer(lua_State *L);
	//int SetTimer(funcname[], interval, repeating);
	//int SetTimerEx(funcname[], interval, repeating, const format[], { Float, _ }:...);
	//int KillTimer(timerid);
	int getTickCount(lua_State *L);
	int getMaxPlayers(lua_State *L);
	//int CallRemoteFunction(const function[], const format[], { Float, _ }:...);
	//int CallLocalFunction(const function[], const format[], { Float, _ }:...);
	int vectorSize(lua_State *L);
	int getPlayerPoolSize(lua_State *L);
	int getVehiclePoolSize(lua_State *L);
	int getActorPoolSize(lua_State *L);

	//Functions - Game
	int setGameModeText(lua_State *L);
	int setTeamCount(lua_State *L);
	int addPlayerClass(lua_State *L);
	int addPlayerClassEx(lua_State *L);
	int addStaticVehicle(lua_State *L);
	int addStaticVehicleEx(lua_State *L);
	int addStaticPickup(lua_State *L);
	int createPickup(lua_State *L);
	int destroyPickup(lua_State *L);
	int showNameTags(lua_State *L);
	int showPlayerMarkers(lua_State *L);
	int gameModeExit(lua_State *L);
	int setWorldTime(lua_State *L);
	int getWeaponName(lua_State *L);
	int enableTirePopping(lua_State *L); // deprecated function
	int enableVehicleFriendlyFire(lua_State *L);
	int allowInteriorWeapons(lua_State *L);
	int setWeather(lua_State *L);
	int setGravity(lua_State *L);
	int allowAdminTeleport(lua_State *L); // deprecated function (maybe?)
	int setDeathDropAmount(lua_State *L); //it doesn't work in the current SA-MP version
	int createExplosion(lua_State *L);
	int enableZoneNames(lua_State *L);
	int usePlayerPedAnims(lua_State *L);		// Will cause the players to use CJ running/walking animations
	int disableInteriorEnterExits(lua_State *L);  // will disable all interior enter/exits in the game.
	int setNameTagDrawDistance(lua_State *L); // Distance at which nametags will start rendering on the client.
	int disableNameTagLOS(lua_State *L); // Disables the nametag Line-Of-Sight checking
	int limitGlobalChatRadius(lua_State *L);
	int limitPlayerMarkerRadius(lua_State *L);

	//Functions - NPC
	int connectNPC(lua_State *L);
	int isPlayerNPC(lua_State *L);

	//Functions - Admin
	int isPlayerAdmin(lua_State *L);
	int kickPlayer(lua_State *L);
	int banPlayer(lua_State *L);
	int banPlayerEx(lua_State *L);
	int sendRconCommand(lua_State *L);
	int getServerVarAsString(lua_State *L);
	int getServerVarAsInt(lua_State *L);
	int getServerVarAsBool(lua_State *L);
	int getPlayerNetworkStats(lua_State *L);
	int getNetworkStats(lua_State *L);
	int getPlayerVersion(lua_State *L); // Returns the SA-MP client revision as reported by the player
	int blockIpAddress(lua_State *L);
	int unBlockIpAddress(lua_State *L);

	//Functions - Network stats
	int getServerTickRate(lua_State *L);
	int netStats_GetConnectedTime(lua_State *L);
	int netStats_MessagesReceived(lua_State *L);
	int netStats_BytesReceived(lua_State *L);
	int netStats_MessagesSent(lua_State *L);
	int netStats_BytesSent(lua_State *L);
	int netStats_MessagesRecvPerSecond(lua_State *L);
	int netStats_PacketLossPercent(lua_State *L);
	int netStats_ConnectionStatus(lua_State *L);
	int netStats_GetIpPort(lua_State *L);

	//Functions - Menu
	int createMenu(lua_State *L);
	int destroyMenu(lua_State *L);
	int addMenuItem(lua_State *L);
	int setMenuColumnHeader(lua_State *L);
	int showMenuForPlayer(lua_State *L);
	int hideMenuForPlayer(lua_State *L);
	int isValidMenu(lua_State *L);
	int disableMenu(lua_State *L);
	int disableMenuRow(lua_State *L);
	int getPlayerMenu(lua_State *L);

	//Functions - TextDraw
	int textDrawCreate(lua_State *L);
	int textDrawDestroy(lua_State *L);
	int textDrawLetterSize(lua_State *L);
	int textDrawTextSize(lua_State *L);
	int textDrawAlignment(lua_State *L);
	int textDrawColor(lua_State *L);
	int textDrawUseBox(lua_State *L);
	int textDrawBoxColor(lua_State *L);
	int textDrawSetShadow(lua_State *L);
	int textDrawSetOutline(lua_State *L);
	int textDrawBackgroundColor(lua_State *L);
	int textDrawFont(lua_State *L);
	int textDrawSetProportional(lua_State *L);
	int textDrawSetSelectable(lua_State *L);
	int textDrawShowForPlayer(lua_State *L);
	int textDrawHideForPlayer(lua_State *L);
	int textDrawShowForAll(lua_State *L);
	int textDrawHideForAll(lua_State *L);
	int textDrawSetString(lua_State *L);
	int textDrawSetPreviewModel(lua_State *L);
	int textDrawSetPreviewRot(lua_State *L);
	int textDrawSetPreviewVehCol(lua_State *L);

	//Functions - GangZone
	int gangZoneCreate(lua_State *L);
	int gangZoneDestroy(lua_State *L);
	int gangZoneShowForPlayer(lua_State *L);
	int gangZoneShowForAll(lua_State *L);
	int gangZoneHideForPlayer(lua_State *L);
	int gangZoneHideForAll(lua_State *L);
	int gangZoneFlashForPlayer(lua_State *L);
	int gangZoneFlashForAll(lua_State *L);
	int gangZoneStopFlashForPlayer(lua_State *L);
	int gangZoneStopFlashForAll(lua_State *L);

	//Functions - TextLabel
	int create3DTextLabel(lua_State *L);
	int delete3DTextLabel(lua_State *L);
	int attach3DTextLabelToPlayer(lua_State *L);
	int attach3DTextLabelToVehicle(lua_State *L);
	int update3DTextLabelText(lua_State *L);

	//Functions - Per-player TextLabel
	int createPlayer3DTextLabel(lua_State *L);
	int deletePlayer3DTextLabel(lua_State *L);
	int updatePlayer3DTextLabelText(lua_State *L);

	//Functions - Dialog
	int showPlayerDialog(lua_State *L);

	//Functions - HTTP
	int lua_HTTP(lua_State *L); //there is no callback argument in sampgdk

	//Functions - Actor
	int createActor(lua_State *L);
	int destroyActor(lua_State *L);
	int isActorStreamedIn(lua_State *L);
	int setActorVirtualWorld(lua_State *L);
	int getActorVirtualWorld(lua_State *L);
	int applyActorAnimation(lua_State *L);
	int clearActorAnimations(lua_State *L);
	int setActorPos(lua_State *L);
	int getActorPos(lua_State *L);
	int setActorFacingAngle(lua_State *L);
	int getActorFacingAngle(lua_State *L);
	int setActorHealth(lua_State *L);
	int getActorHealth(lua_State *L);
	int setActorInvulnerable(lua_State *L);
	int isActorInvulnerable(lua_State *L);
	int isValidActor(lua_State *L);

	//Functions - Object
	int createObject(lua_State *L);
	int attachObjectToVehicle(lua_State *L);
	int attachObjectToObject(lua_State *L);
	int attachObjectToPlayer(lua_State *L);
	int setObjectPos(lua_State *L);
	int getObjectPos(lua_State *L);
	int setObjectRot(lua_State *L);
	int getObjectRot(lua_State *L);
	int getObjectModel(lua_State *L);
	int setObjectNoCameraCol(lua_State *L);
	int isValidObject(lua_State *L);
	int destroyObject(lua_State *L);
	int moveObject(lua_State *L);
	int stopObject(lua_State *L);
	int isObjectMoving(lua_State *L);
	int editObject(lua_State *L);
	int editPlayerObject(lua_State *L);
	int selectObject(lua_State *L);
	int cancelEdit(lua_State *L);
	int createPlayerObject(lua_State *L);
	int attachPlayerObjectToVehicle(lua_State *L);
	int setPlayerObjectPos(lua_State *L);
	int getPlayerObjectPos(lua_State *L);
	int setPlayerObjectRot(lua_State *L);
	int getPlayerObjectRot(lua_State *L);
	int getPlayerObjectModel(lua_State *L);
	int setPlayerObjectNoCameraCol(lua_State *L);
	int isValidPlayerObject(lua_State *L);
	int destroyPlayerObject(lua_State *L);
	int movePlayerObject(lua_State *L);
	int stopPlayerObject(lua_State *L);
	int isPlayerObjectMoving(lua_State *L);
	int attachPlayerObjectToPlayer(lua_State *L);
	int setObjectMaterial(lua_State *L);
	int setPlayerObjectMaterial(lua_State *L);
	int setObjectMaterialText(lua_State *L);
	int setPlayerObjectMaterialText(lua_State *L);
	int setObjectsDefaultCameraCol(lua_State *L);

	//Functions - Vehicle
	int createVehicle(lua_State *L);
	int destroyVehicle(lua_State *L);
	int isVehicleStreamedIn(lua_State *L);
	int getVehiclePos(lua_State *L);
	int setVehiclePos(lua_State *L);
	int getVehicleZAngle(lua_State *L);
	int getVehicleRotationQuat(lua_State *L);
	int getVehicleDistanceFromPoint(lua_State *L);
	int setVehicleZAngle(lua_State *L);
	int setVehicleParamsForPlayer(lua_State *L);
	int manualVehicleEngineAndLights(lua_State *L);
	int setVehicleParamsEx(lua_State *L);
	int getVehicleParamsEx(lua_State *L);
	int getVehicleParamsSirenState(lua_State *L);
	int setVehicleParamsCarDoors(lua_State *L);
	int getVehicleParamsCarDoors(lua_State *L);
	int setVehicleParamsCarWindows(lua_State *L);
	int getVehicleParamsCarWindows(lua_State *L);
	int setVehicleToRespawn(lua_State *L);
	int linkVehicleToInterior(lua_State *L);
	int addVehicleComponent(lua_State *L);
	int removeVehicleComponent(lua_State *L);
	int changeVehicleColor(lua_State *L);
	int changeVehiclePaintjob(lua_State *L);
	int setVehicleHealth(lua_State *L);
	int getVehicleHealth(lua_State *L);
	int attachTrailerToVehicle(lua_State *L);
	int detachTrailerFromVehicle(lua_State *L);
	int isTrailerAttachedToVehicle(lua_State *L);
	int getVehicleTrailer(lua_State *L);
	int setVehicleNumberPlate(lua_State *L);
	int getVehicleModel(lua_State *L);
	int getVehicleComponentInSlot(lua_State *L); // There is 1 slot for each CARMODTYPE_*
	int getVehicleComponentType(lua_State *L); // Find CARMODTYPE_* for component id
	int repairVehicle(lua_State *L); // Repairs the damage model and resets the health
	int getVehicleVelocity(lua_State *L);
	int setVehicleVelocity(lua_State *L);
	int setVehicleAngularVelocity(lua_State *L);
	int getVehicleDamageStatus(lua_State *L);
	int updateVehicleDamageStatus(lua_State *L);
	int getVehicleModelInfo(lua_State *L);
	int setVehicleVirtualWorld(lua_State *L);
	int getVehicleVirtualWorld(lua_State *L);

	//Functions - Players
	int setSpawnInfo(lua_State *L);
	int spawnPlayer(lua_State *L);
	int setPlayerPos(lua_State *L);
	int setPlayerPosFindZ(lua_State *L);
	int getPlayerPos(lua_State *L);
	int setPlayerFacingAngle(lua_State *L);
	int getPlayerFacingAngle(lua_State *L);
	int isPlayerInRangeOfPoint(lua_State *L);
	int getPlayerDistanceFromPoint(lua_State *L);
	int isPlayerStreamedIn(lua_State *L);
	int setPlayerInterior(lua_State *L);
	int getPlayerInterior(lua_State *L);
	int setPlayerHealth(lua_State *L);
	int getPlayerHealth(lua_State *L);
	int setPlayerArmour(lua_State *L);
	int getPlayerArmour(lua_State *L);
	int setPlayerAmmo(lua_State *L);
	int getPlayerAmmo(lua_State *L);
	int getPlayerWeaponState(lua_State *L);
	int getPlayerTargetPlayer(lua_State *L);
	int getPlayerTargetActor(lua_State *L);
	int setPlayerTeam(lua_State *L);
	int getPlayerTeam(lua_State *L);
	int setPlayerScore(lua_State *L);
	int getPlayerScore(lua_State *L);
	int getPlayerDrunkLevel(lua_State *L);
	int setPlayerDrunkLevel(lua_State *L);
	int setPlayerColor(lua_State *L);
	int getPlayerColor(lua_State *L);
	int setPlayerSkin(lua_State *L);
	int getPlayerSkin(lua_State *L);
	int givePlayerWeapon(lua_State *L);
	int resetPlayerWeapons(lua_State *L);
	int setPlayerArmedWeapon(lua_State *L);
	int getPlayerWeaponData(lua_State *L);
	int givePlayerMoney(lua_State *L);
	int resetPlayerMoney(lua_State *L);
	int setPlayerName(lua_State *L);
	int getPlayerMoney(lua_State *L);
	int getPlayerState(lua_State *L);
	int getPlayerIp(lua_State *L);
	int getPlayerPing(lua_State *L);
	int getPlayerWeapon(lua_State *L);
	int getPlayerKeys(lua_State *L);
	int getPlayerName(lua_State *L);
	int setPlayerTime(lua_State *L);
	int getPlayerTime(lua_State *L);
	int togglePlayerClock(lua_State *L);
	int setPlayerWeather(lua_State *L);
	int forceClassSelection(lua_State *L);
	int setPlayerWantedLevel(lua_State *L);
	int getPlayerWantedLevel(lua_State *L);
	int setPlayerFightingStyle(lua_State *L);
	int getPlayerFightingStyle(lua_State *L);
	int setPlayerVelocity(lua_State *L);
	int getPlayerVelocity(lua_State *L);
	int playCrimeReportForPlayer(lua_State *L);
	int playAudioStreamForPlayer(lua_State *L);
	int stopAudioStreamForPlayer(lua_State *L);
	int setPlayerShopName(lua_State *L);
	int setPlayerSkillLevel(lua_State *L);
	int getPlayerSurfingVehicleID(lua_State *L);
	int getPlayerSurfingObjectID(lua_State *L);
	int removeBuildingForPlayer(lua_State *L);
	int getPlayerLastShotVectors(lua_State *L);
	int setPlayerAttachedObject(lua_State *L);
	int removePlayerAttachedObject(lua_State *L);
	int isPlayerAttachedObjectSlotUsed(lua_State *L);
	int editAttachedObject(lua_State *L);
	int createPlayerTextDraw(lua_State *L);
	int playerTextDrawDestroy(lua_State *L);
	int playerTextDrawLetterSize(lua_State *L);
	int playerTextDrawTextSize(lua_State *L);
	int playerTextDrawAlignment(lua_State *L);
	int playerTextDrawColor(lua_State *L);
	int playerTextDrawUseBox(lua_State *L);
	int playerTextDrawBoxColor(lua_State *L);
	int playerTextDrawSetShadow(lua_State *L);
	int playerTextDrawSetOutline(lua_State *L);
	int playerTextDrawBackgroundColor(lua_State *L);
	int playerTextDrawFont(lua_State *L);
	int playerTextDrawSetProportional(lua_State *L);
	int playerTextDrawSetSelectable(lua_State *L);
	int playerTextDrawShow(lua_State *L);
	int playerTextDrawHide(lua_State *L);
	int playerTextDrawSetString(lua_State *L);
	int playerTextDrawSetPreviewModel(lua_State *L);
	int playerTextDrawSetPreviewRot(lua_State *L);
	int playerTextDrawSetPreviewVehCol(lua_State *L);
	int setPVarInt(lua_State *L);
	int getPVarInt(lua_State *L);
	int setPVarString(lua_State *L);
	int getPVarString(lua_State *L);
	int setPVarFloat(lua_State *L);
	int getPVarFloat(lua_State *L);
	int deletePVar(lua_State *L);
	int getPVarsUpperIndex(lua_State *L);
	int getPVarNameAtIndex(lua_State *L);
	int getPVarType(lua_State *L);
	int setPlayerChatBubble(lua_State *L);
	int putPlayerInVehicle(lua_State *L);
	int getPlayerVehicleID(lua_State *L);
	int getPlayerVehicleSeat(lua_State *L);
	int removePlayerFromVehicle(lua_State *L);
	int togglePlayerControllable(lua_State *L);
	int playerPlaySound(lua_State *L);
	int applyAnimation(lua_State *L);
	int clearAnimations(lua_State *L);
	int getPlayerAnimationIndex(lua_State *L); // return the index of any running applied animations (0 if none are running)
	int getAnimationName(lua_State *L); // get the animation lib/name for the index
	int getPlayerSpecialAction(lua_State *L);
	int setPlayerSpecialAction(lua_State *L);
	int disableRemoteVehicleCollisions(lua_State *L);
	int setPlayerCheckpoint(lua_State *L);
	int disablePlayerCheckpoint(lua_State *L);
	int setPlayerRaceCheckpoint(lua_State *L);
	int disablePlayerRaceCheckpoint(lua_State *L);
	int setPlayerWorldBounds(lua_State *L);
	int setPlayerMarkerForPlayer(lua_State *L);
	int showPlayerNameTagForPlayer(lua_State *L);
	int setPlayerMapIcon(lua_State *L);
	int removePlayerMapIcon(lua_State *L);
	int allowPlayerTeleport(lua_State *L);
	int setPlayerCameraPos(lua_State *L);
	int setPlayerCameraLookAt(lua_State *L);
	int setCameraBehindPlayer(lua_State *L);
	int getPlayerCameraPos(lua_State *L);
	int getPlayerCameraFrontVector(lua_State *L);
	int getPlayerCameraMode(lua_State *L);
	int enablePlayerCameraTarget(lua_State *L);
	int getPlayerCameraTargetObject(lua_State *L);
	int getPlayerCameraTargetVehicle(lua_State *L);
	int getPlayerCameraTargetPlayer(lua_State *L);
	int getPlayerCameraTargetActor(lua_State *L);
	int getPlayerCameraAspectRatio(lua_State *L);
	int getPlayerCameraZoom(lua_State *L);
	int attachCameraToObject(lua_State *L);
	int attachCameraToPlayerObject(lua_State *L);
	int interpolateCameraPos(lua_State *L);
	int interpolateCameraLookAt(lua_State *L);
	int isPlayerConnected(lua_State *L);
	int isPlayerInVehicle(lua_State *L);
	int isPlayerInAnyVehicle(lua_State *L);
	int isPlayerInCheckpoint(lua_State *L);
	int isPlayerInRaceCheckpoint(lua_State *L);
	int setPlayerVirtualWorld(lua_State *L);
	int getPlayerVirtualWorld(lua_State *L);
	int enableStuntBonusForPlayer(lua_State *L);
	int enableStuntBonusForAll(lua_State *L);
	int togglePlayerSpectating(lua_State *L);
	int playerSpectatePlayer(lua_State *L);
	int playerSpectateVehicle(lua_State *L);
	int startRecordingPlayerData(lua_State *L);
	int stopRecordingPlayerData(lua_State *L);
	int selectTextDraw(lua_State *L); // enables the mouse so the player can select a textdraw
	int cancelSelectTextDraw(lua_State *L);	// cancel textdraw selection with the mouse
	int createExplosionForPlayer(lua_State *L);
}

#endif