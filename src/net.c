#include "caulk.h"
#include "raylib.h"

#include "game.h"
#include "net.h"
#include "text.h"

#define REFRESH_RATE (5.0)
#define SECRET_KEY "PoleChudes"
#define SECRET_VALUE "Baraban!!!"

extern bool caulkInit;

static float lastRefresh = -1.0;
static CSteamID lobbies[MAX_LOBBIES] = {0};
static CSteamID curLobby = 0;
static size_t lobbyCount = 0;

static void onLobbyCreate(void* rawData) {
	LobbyCreated_t* data = rawData;
	if (data->m_eResult != k_EResultOK)
		return;
	curLobby = ((LobbyEnter_t*)data)->m_ulSteamIDLobby;
}

static void onLobbyEnter(void* data) {
	curLobby = ((LobbyEnter_t*)data)->m_ulSteamIDLobby;
	caulk_SteamMatchmaking_SetLobbyData(curLobby, SECRET_KEY, SECRET_VALUE);
	caulk_SteamMatchmaking_SetLobbyMemberData(curLobby, "name", caulk_SteamFriends_GetPersonaName());
}

void netInit() {
	caulk_Register(LobbyCreated_t_iCallback, onLobbyCreate);
	caulk_Register(LobbyEnter_t_iCallback, onLobbyEnter);
}

static void resolveLobbyList(void* rawData, bool shit) {
	LobbyMatchList_t* data = rawData;
	if (shit)
		lobbyCount = 0;
	else
		lobbyCount = data->m_nLobbiesMatching;
	for (size_t idx = 0; idx < lobbyCount; idx++)
		lobbies[idx] = caulk_SteamMatchmaking_GetLobbyByIndex(idx);
}

CSteamID getCurLobby() {
	return curLobby;
}

CSteamID* getLobbies() {
	const float time = GetTime();
	if (lastRefresh < 0.0 || time - lastRefresh >= REFRESH_RATE) {
		caulk_SteamMatchmaking_AddRequestLobbyListDistanceFilter(k_ELobbyDistanceFilterWorldwide);
		caulk_SteamMatchmaking_AddRequestLobbyListResultCountFilter(MAX_LOBBIES);
		caulk_SteamMatchmaking_AddRequestLobbyListStringFilter(
		    SECRET_KEY, SECRET_VALUE, k_ELobbyComparisonEqual
		);
		caulk_Resolve(caulk_SteamMatchmaking_RequestLobbyList(), resolveLobbyList);
		lastRefresh = time;
	}
	return lobbies;
}

size_t getLobbyCount() {
	return lobbyCount;
}

void quitLobby() {
	stopVeda();
	if (curLobby)
		caulk_SteamMatchmaking_LeaveLobby(curLobby);
	curLobby = 0;
	gameReset();
}

void joinLobby(size_t idx) {
	quitLobby();
	if (idx < lobbyCount)
		caulk_SteamMatchmaking_JoinLobby(lobbies[idx]);
}

void hostLobby(const char* name) {
	quitLobby();
	caulk_SteamMatchmaking_CreateLobby(k_ELobbyTypePublic, MAX_PLAYERS);
}

size_t getPlayerCount() {
	if (!curLobby)
		return 0;
	const size_t count = caulk_SteamMatchmaking_GetNumLobbyMembers(curLobby);
	return count > MAX_PLAYERS ? MAX_PLAYERS : count;
}

bool weMaster() {
	return weOnline() && caulk_SteamMatchmaking_GetLobbyOwner(curLobby) == caulk_SteamUser_GetSteamID();
}

bool weOnline() {
	return curLobby != 0;
}
