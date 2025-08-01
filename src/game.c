#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caulk.h"
#include "raylib.h"

#include "game.h"
#include "net.h"
#include "text.h"
#include "utils.h"

enum {
	St_Null = 0,
	St_Welcome,
	St_Guess,
};

enum {
	Msg_Update,
};

static bool started = false;
static int states[MAX_PLAYERS] = {0};
static float transTimer = 0.0;

static void receiveMsg(void* cbData) {
	static char buf[8192] = {0};
	size_t size = caulk_SteamMatchmaking_GetLobbyChatEntry(
	    getCurLobby(), ((LobbyChatMsg_t*)cbData)->m_iChatID, NULL, buf, LENGTH(buf), NULL
	);

	if (size < 2)
		return;
	const char* rawData = buf + 2;
	started = true;

	switch (*(uint16_t*)buf) {
	case Msg_Update:
		for (size_t i = 0; i < MAX_PLAYERS; i++)
			states[i] = ((int*)rawData)[i];
		break;
	default:
		break;
	}
}

void gameInit() {
	gameReset();
	transTimer = GetTime();
	caulk_Register(LobbyChatMsg_t_iCallback, receiveMsg);
}

void gameReset() {
	started = false;
	for (size_t i = 0; i < MAX_PLAYERS; i++)
		states[i] = St_Null;
}

static void updateGameState() {
	static char buf[512] = {0};
	*((uint16_t*)buf) = Msg_Update;
	memcpy(buf + 2, states, MAX_PLAYERS * sizeof(*states));

	caulk_SteamMatchmaking_SendLobbyChatMsg(getCurLobby(), buf, 2 + MAX_PLAYERS * sizeof(*states));
}

#define Trans(_from, _to) if (from == (_from) && to == (_to))

static int runStateTransition(int player, int from, int to) {
	static char buf[LINE_MAX] = {0};
	setVedaet(false);

	// printf("i=%d f=%d t=%d\n", player, from, to);

	Trans(St_Null, St_Welcome) {
		CSteamID id = caulk_SteamMatchmaking_GetLobbyMemberByIndex(getCurLobby(), player);
		if (!id)
			return St_Null;
		const char* name = caulk_SteamMatchmaking_GetLobbyMemberData(getCurLobby(), id, "name");

		clearLines();
		snprintf(buf, LENGTH(buf), "Welcome, %s!", name);
		setLine(0, buf);

		transTimer += 3.0;
		setVedaet(true);
		return St_Guess;
	}

	Trans(St_Welcome, St_Guess) {
		clearLines();
		setLine(0, "That's all for now");
		transTimer += 5.0;
		return St_Guess;
	}

	return St_Null;
}

#undef Trans

static void processStateUpdates() {
	static int old[MAX_PLAYERS] = {0}, transitions[MAX_PLAYERS] = {0};

	const float time = GetTime();
	if (transTimer - time > 1e-5)
		return;
	transTimer = time;

	for (size_t i = 0; i < getPlayerCount(); i++) {
		if (transitions[i] == St_Null) {
			transitions[i] = runStateTransition(i, old[i], states[i]);
			return;
		}
	}

	for (size_t i = 0; i < getPlayerCount(); i++) {
		old[i] = states[i];
		states[i] = transitions[i];
		transitions[i] = St_Null;
	}
}

void gameStart() {
	for (size_t i = 0; i < getPlayerCount(); i++)
		states[i] = St_Welcome;
	started = true;
	processStateUpdates();
}

void gameUpdate() {
	if (started && weMaster())
		updateGameState();
	processStateUpdates();
}

bool weStarted() {
	return started;
}
