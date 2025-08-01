#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "caulk.h"

#define MAX_LOBBIES (4)

void netInit();
bool weOnline();

void joinLobby(size_t), quitLobby();
void hostLobby(const char*);

CSteamID* getLobbies();
size_t getLobbyCount();

size_t getPlayerCount();
