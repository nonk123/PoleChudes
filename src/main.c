#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caulk.h"
#include "raylib.h"

#include "text.h"
#include "utils.h"

bool quit = false, quitOnEsc = true, caulkInit = false;
bool vedaet = false;
bool weGucci = false, weOnline = false; // networking...

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
		if (!strcmp(CUR_ARG, "--nodisclaimers"))
			showDisclaimers = false;
		NEXT_ARG;
	}

#undef CUR_ARG
#undef NEXT_ARG
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

	while (!quit && !WindowShouldClose()) {
		caulk_Dispatch();

		if (curDisclaimer != NULL) {
			disclaim();
			continue;
		}

		if (quitOnEsc && IsKeyDown(KEY_ESCAPE))
			quit = true;
		if (IsKeyPressed(KEY_V)) // debug....
			vedaet = !vedaet;

		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode2D(camera());

		DrawTexture(background, 0, 0, WHITE);
		DrawTexture(ved[vedaet ? (int)(GetTime() * 5.0f) % 2 : 0], 488, 329, WHITE);
		if (!weOnline)
			writeLine(0, "bitch, you're OFFLINE");

		EndMode2D();
		EndDrawing();
	}

	if (caulkInit)
		caulk_Shutdown();

	CloseAudioDevice();
	CloseWindow();

	return EXIT_SUCCESS;
}
