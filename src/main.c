#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caulk.h"
#include "raylib.h"

#include "game.h"
#include "net.h"
#include "text.h"
#include "utils.h"

bool quit = false, caulkInit = false;

Font font;
static Texture2D background, ved[2];

#define DISCLAIMER_INTERVAL (3.0)
static float disclElapsed = 0.0f;
static bool showDisclaimers = true;
static Texture2D disclaimers[2], *curDisclaimer = NULL;

static void loadAssets() {
#define ROOT "assets/"
	background = LoadTexture(ROOT "fon_pole.png");
	ved[0] = LoadTexture(ROOT "ved1.PNG");
	ved[1] = LoadTexture(ROOT "ved2.PNG");

	font = LoadFont(ROOT "fnt/dos.ttf");
	font.baseSize = 32;
	SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);

	disclaimers[0] = LoadTexture(ROOT "discl1.png");
	disclaimers[1] = LoadTexture(ROOT "discl2.png");
	if (showDisclaimers)
		curDisclaimer = &disclaimers[0];
#undef ROOT
}

int zoom = 2;
static Camera2D camera() {
	return (Camera2D){
	    .offset = {0.0, 0.0},
	    .rotation = 0.0,
	    .target = {0.0, 0.0},
	    .zoom = zoom,
	};
}

static void disclaim() {
	BeginDrawing();
	BeginMode2D(camera());
	ClearBackground(BLACK);
	DrawTexture(*curDisclaimer, 0, 0, WHITE);
	EndMode2D();
	EndDrawing();

	if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE))
		curDisclaimer++;

	float time = GetTime();
	if (time - disclElapsed >= DISCLAIMER_INTERVAL) {
		disclElapsed = time;
		curDisclaimer++;
	}

	if (curDisclaimer - disclaimers >= LENGTH(disclaimers))
		curDisclaimer = NULL;
}

static void parseArgs(int argc, char* argv[]) {
#define NEXT_ARG                                                                                                       \
	do {                                                                                                           \
		argv++;                                                                                                \
		argc--;                                                                                                \
	} while (0)
#define CUR_ARG (*argv)

	NEXT_ARG;
	while (argc > 0) {
		if (!strcmp(CUR_ARG, "--nozoom"))
			zoom = 1;
		else if (!strcmp(CUR_ARG, "--nodisclaimers"))
			showDisclaimers = false;
		NEXT_ARG;
	}

#undef CUR_ARG
#undef NEXT_ARG
}

static void menu() {
	char buf[512] = {0};
	if (weOnline()) {
		clearLines();
		snprintf(buf, LENGTH(buf), "%d players", (int)getPlayerCount());
		setLine(0, buf);
		setLine(1, "SPACE to start");

		if (weMaster() && IsKeyPressed(KEY_SPACE))
			gameStart();
	} else {
		clearLines();
		snprintf(buf, LENGTH(buf), "%d lobbies", (int)getLobbyCount());
		setLine(0, buf);
		setLine(1, "J join");
		setLine(2, "H create lobby");

		if (IsKeyPressed(KEY_H))
			hostLobby("test");
		if (IsKeyPressed(KEY_J))
			joinLobby(0);
	}
}

int main(int argc, char* argv[]) {
	parseArgs(argc, argv);

	InitWindow(640 * zoom, 480 * zoom, "Pole Chudes: Revisited");
	InitAudioDevice();

	SetExitKey(KEY_NULL);
	SetTargetFPS(60);

	if (!(caulkInit = caulk_Init()))
		printf("Caulk failed to init!!! SHIT!!!!!!!\n");

	loadAssets();
	netInit();
	gameInit();

	while (!quit && !WindowShouldClose()) {
		getLobbies();
		caulk_Dispatch();

		if (curDisclaimer != NULL) {
			disclaim();
			continue;
		}

		if (!weStarted())
			menu();

		if (IsKeyPressed(KEY_Q))
			quit = true;
		if (IsKeyPressed(KEY_V)) // debug....
			setVedaet(!isVedaet());

		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode2D(camera());

		DrawTexture(background, 0, 0, WHITE);
		DrawTexture(ved[isVedaet() ? (int)(GetTime() * 5.0f) % 2 : 0], 488, 329, WHITE);

		if (weOnline() && IsKeyPressed(KEY_ESCAPE)) {
			quitLobby();
		}

		gameUpdate();
		drawTextLines();

		EndMode2D();
		EndDrawing();
	}

	if (caulkInit)
		caulk_Shutdown();

	CloseAudioDevice();
	CloseWindow();

	return EXIT_SUCCESS;
}
