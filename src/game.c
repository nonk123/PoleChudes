#include <stdbool.h>
#include <stddef.h>
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
	St_Null,
	St_Welcome,
	St_Guess,
};

enum {
	Msg_History,
	Msg_Heartbeat,
};

struct Record {
	int8_t player; // -1 means free history record
	uint8_t transition, params[4];
};

static int started = 0;

#define HIST_MAX (2048)
static uint16_t histPos = 0;
static struct Record hist[HIST_MAX] = {0}, *pushHist();

static void receiveMsg(void* cbData) {
	static uint8_t buf[4096] = {0};
	size_t packetSize = caulk_SteamMatchmaking_GetLobbyChatEntry(
	    getCurLobby(), ((LobbyChatMsg_t*)cbData)->m_iChatID, NULL, buf, LENGTH(buf), NULL
	);

	if (packetSize < 3 || packetSize > LENGTH(buf))
		return;

	const uint8_t msg = buf[0];
	uint16_t sizePayload = buf[1] | (buf[2] << 8);
#ifdef __BIG_ENDIAN__
	sizePayload = (sizePayload >> 8) | (sizePayload << 8);
#endif
	const uint8_t* rawData = buf + 3;

	switch (msg) {
	case Msg_History:
		if (weMaster())
			break;

		const size_t netHistCount = sizePayload / 6;
		const uint8_t* reverse = rawData + (sizePayload - 1);
		for (size_t i = 0; i < netHistCount; i++) {
			for (int j = 3; j >= 0; j--)
				hist[i].params[j] = *reverse--;
			hist[i].transition = *reverse--;
			hist[i].player = *reverse--;
		}

		break;
	case Msg_Heartbeat:
		started = 60;
		break;
	default:
		break;
	}
}

void gameInit() {
	gameReset();
	caulk_Register(LobbyChatMsg_t_iCallback, receiveMsg);
}

static void sendMessage(uint8_t type, const char* data, size_t size) {
	static char buf[1024] = {0};
	if (size + 3 >= LENGTH(buf)) {
		printf("HUGE CHUNKUS MESSAGE IGNORED!!!!\n");
		return;
	}

	if (data == NULL)
		size = 0;
	else
		memcpy(buf + 3, data, size);

	const uint16_t sizePayload = size;
#ifdef __BIG_ENDIAN__
	sizePayload = (sizePayload >> 8) | (sizePayload << 8);
#endif
	((uint8_t*)buf)[0] = type;
	((uint8_t*)buf)[1] = (uint8_t)((sizePayload & 0x00FF) >> 0);
	((uint8_t*)buf)[2] = (uint8_t)((sizePayload & 0xFF00) >> 8);

	caulk_SteamMatchmaking_SendLobbyChatMsg(getCurLobby(), buf, 3 + size);
}

static void sendGameState() {
	if (!weMaster())
		return;

	static char buf[512] = {0};
	char* ptr = buf;
	size_t count = 0;

	for (size_t i = 0; i <= histPos; i++) {
		if (hist[i].player == -1)
			break;
		*ptr++ = hist[i].player;
		*ptr++ = hist[i].transition;
		for (int j = 0; j < 4; j++)
			*ptr++ = ((char*)&hist[i].params)[j];
		count++;
	}

	sendMessage(Msg_History, buf, (size_t)(6 * count));
}

static void sendHeartbeat() {
	if (weMaster())
		sendMessage(Msg_Heartbeat, NULL, 0);
	else
		started--;
}

#define Trans(_from, _to) if (from == (_from) && to == (_to))

static uint8_t transitionMaster(int player, int from, int to, uint8_t* params) {
	Trans(St_Null, St_Welcome) {
		return St_Guess;
	}
	Trans(St_Welcome, St_Guess) {
		return St_Guess;
	}
	Trans(St_Guess, St_Guess) {
		params[0] = !GetRandomValue(0, 60 * 3 - 1);
		return St_Guess;
	}
	return St_Null;
}

static void transitionSlave(int player, int from, int to, const uint8_t* params) {
	static char buf[VEDA_MAX] = {0};

	Trans(St_Null, St_Welcome) {
		CSteamID id = caulk_SteamMatchmaking_GetLobbyMemberByIndex(getCurLobby(), player);
		if (!id)
			return;
		const char* name = caulk_SteamMatchmaking_GetLobbyMemberData(getCurLobby(), id, "name");

		clearLines();
		snprintf(buf, LENGTH(buf), "Welcome, %s!", name);
		vedaem(2.0, buf);
	}

	Trans(St_Welcome, St_Guess) {
		clearLines();
		vedaem(2.0, "That's all for now");
	}

	Trans(St_Guess, St_Guess) {
		if (params[0])
			vedaem(1.2, "Ahem");
	}
}

#undef Trans

static struct Record* pushHist() {
	for (size_t i = histPos; i < HIST_MAX; i++)
		if (hist[i].player == -1) {
			memset(hist[i].params, 0, LENGTH(hist[i].params));
			return &hist[i];
		}

	printf("REACHED HISTORY LIMIT; CATCHING ON FIRE **NOW**!!!!\n");
	return NULL;
}

static int roundRobin = 0;
static void tickStateMachine() {
	if (isVedaet())
		return;

	size_t player = roundRobin % getPlayerCount();
	uint8_t params[4] = {0};

	if (hist[histPos].player == -1) {
		sendGameState();
		return;
	}

	uint8_t from = St_Null, to = St_Null, transition = St_Null;

	size_t backtrack = histPos;
	while (backtrack >= 0)
		if (hist[backtrack].player == player) {
			to = hist[backtrack].transition;
			memcpy(params, hist[backtrack].params, LENGTH(params));
			backtrack--;
			break;
		} else
			backtrack--;
	while (backtrack >= 0)
		if (hist[backtrack].player == player) {
			from = hist[backtrack].transition;
			break;
		} else
			backtrack--;

	if (to != St_Null) {
		if (weMaster())
			transition = transitionMaster(player, from, to, params);
		transitionSlave(player, from, to, params);
		histPos++;
	}

	// FIXME: probably the game-ending scenario?
	if (weMaster() && transition != St_Null) {
		struct Record* ptr = pushHist();
		ptr->player = player;
		ptr->transition = transition;
		memcpy(ptr->params, params, LENGTH(params));
		roundRobin++;
		sendGameState();
	}
}

void gameReset() {
	started = 0;
	roundRobin = 0;
	histPos = 0;

	for (size_t i = 0; i < HIST_MAX; i++)
		hist[i].player = -1;

	for (size_t i = 0; i < getPlayerCount(); i++) {
		struct Record* ptr = pushHist();
		ptr->player = i;
		ptr->transition = St_Welcome;
	}
}

void gameStart() {
	gameReset();
	sendGameState();
	started = 1;
}

void gameUpdate() {
	if (started) {
		tickStateMachine();
		if (!((int)(GetTime() * 10.0) % 30))
			sendHeartbeat();
	}
}

bool weStarted() {
	return started > 0;
}
