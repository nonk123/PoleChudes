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

struct History {
	struct History* previous;
	uint8_t player, transition;
};

static int started = 0;

#define HIST_MAX (2048)
static size_t histCounter = 0;
static struct History *hist = NULL, histMem[HIST_MAX] = {0};
static void pushHist();

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

		const size_t netHistCount = sizePayload / 2;

		struct History* counter = hist;
		size_t ourHistCount = 0;
		while (counter != NULL) {
			ourHistCount++;
			counter = counter->previous;
		}

		int add = netHistCount - ourHistCount;
		if (add <= 0)
			break;

		const uint8_t* reverse = rawData + (2 * add - 1);
		while (add-- > 0) {
			pushHist();
			hist->transition = *reverse--;
			hist->player = *reverse--;
		}

		break;
	case Msg_Heartbeat:
		printf("BISH!!!\n");
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

	struct History* cur = hist;
	size_t count = 0;

	while (cur != NULL) {
		*ptr++ = cur->player;
		*ptr++ = cur->transition;
		cur = cur->previous;
		count++;
	}

	sendMessage(Msg_History, buf, (size_t)(2 * count));
}

static void sendHeartbeat() {
	if (weMaster())
		sendMessage(Msg_Heartbeat, NULL, 0);
	else
		started--;
}

#define Trans(_from, _to) if (from == (_from) && to == (_to))

static uint8_t runStateTransition(int player, int from, int to) {
	static char buf[VEDA_MAX] = {0};

	Trans(St_Null, St_Welcome) {
		CSteamID id = caulk_SteamMatchmaking_GetLobbyMemberByIndex(getCurLobby(), player);
		if (!id)
			goto noop;
		const char* name = caulk_SteamMatchmaking_GetLobbyMemberData(getCurLobby(), id, "name");

		clearLines();
		snprintf(buf, LENGTH(buf), "Welcome, %s!", name);
		vedaem(2.0, buf);

		return St_Guess;
	}

	Trans(St_Welcome, St_Guess) {
		clearLines();
		vedaem(2.0, "That's all for now");
		return St_Guess;
	}

	Trans(St_Guess, St_Guess) {
		if (!GetRandomValue(0, 60 * 3 - 1))
			vedaem(1.2, "Ahem");
		return St_Guess;
	}

noop:
	return St_Null;
}

#undef Trans

static void pushHist() {
	if (histCounter == HIST_MAX) {
		printf("REACHED HISTORY LIMIT; CATCHING ON FIRE **NOW**!!!!\n");
		return;
	}

	struct History* next = &histMem[histCounter++];
	next->previous = hist;
	hist = next;
}

static int roundRobin = 0;
static void tickStateMachine() {
	if (isVedaet())
		return;

	size_t playerIdx = roundRobin % getPlayerCount();
	uint8_t from = St_Null, to = St_Null, transition = St_Null;

	struct History* ptr = hist;
	for (; ptr != NULL; ptr = ptr->previous)
		if (ptr->player == playerIdx) {
			to = ptr->transition;
			ptr = ptr->previous;
			break;
		}
	for (; ptr != NULL; ptr = ptr->previous)
		if (ptr->player == playerIdx) {
			from = ptr->transition;
			break;
		}

	if (to != St_Null)
		transition = runStateTransition(playerIdx, from, to);

	// FIXME: probably the game-ending scenario?
	if (transition != St_Null) {
		pushHist();
		hist->player = playerIdx;
		hist->transition = transition;
		roundRobin++;
	}
}

void gameReset() {
	started = 0;
	roundRobin = 0;
	histCounter = 0;
	hist = NULL;

	for (size_t i = 0; i < HIST_MAX; i++)
		histMem[i].previous = NULL;

	for (size_t i = 0; i < getPlayerCount(); i++) {
		pushHist();
		hist->player = i;
		hist->transition = St_Welcome;
	}
}

void gameStart() {
	gameReset();
	started = 1;
}

void gameUpdate() {
	sendGameState();
	if (started) {
		tickStateMachine();
		sendHeartbeat();
	}
}

bool weStarted() {
	return started > 0;
}
