#include <stdio.h>

#include "caulk.h"
#include "raylib.h"

#include "net.h"

#define REFRESH_RATE (8.0)
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
	if (curLobby)
		caulk_SteamMatchmaking_LeaveLobby(curLobby);
	curLobby = 0;
}

void joinLobby(size_t idx) {
	quitLobby();
	if (idx < lobbyCount)
		caulk_SteamMatchmaking_JoinLobby(lobbies[idx]);
}

void hostLobby(const char* name) {
	quitLobby();
	caulk_SteamMatchmaking_CreateLobby(k_ELobbyTypePublic, 3);
}

size_t getPlayerCount() {
	if (!curLobby)
		return 0;
	return caulk_SteamMatchmaking_GetNumLobbyMembers(curLobby);
}

bool weOnline() {
	return curLobby != 0;
}
