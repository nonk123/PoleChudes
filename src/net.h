#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "caulk.h"

#define MAX_LOBBIES (4)
#define MAX_PLAYERS (3)

void netInit();
bool weOnline(), weMaster();

void joinLobby(size_t), quitLobby();
void hostLobby(const char*);

CSteamID getCurLobby(), *getLobbies();
size_t getLobbyCount();

size_t getPlayerCount();
