#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "caulk.h"
#include "raylib.h"

bool quit = false, quitOnEsc = true, caulkInit = false;
bool vedaet = false;

static Texture2D background, ved[2];

static void loadAssets() {
#define ROOT "assets/"
	background = LoadTexture(ROOT "fon_pole.png");
	ved[0] = LoadTexture(ROOT "ved1.PNG");
	ved[1] = LoadTexture(ROOT "ved2.PNG");
#undef ROOT
}

int main(int argc, char* argv[]) {
	InitWindow(640, 480, "Pole Chudes: Revisited");
	InitAudioDevice();

	SetExitKey(KEY_NULL);
	SetTargetFPS(60);

	if (!(caulkInit = caulk_Init()))
		printf("Caulk failed to init!!! SHIT!!!!!!!\n");

	loadAssets();

	while (!quit && !WindowShouldClose()) {
		caulk_Dispatch();

		if (quitOnEsc && IsKeyDown(KEY_ESCAPE))
			quit = true;
		if (IsKeyPressed(KEY_SPACE))
			vedaet = !vedaet;

		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawTexture(background, 0, 0, WHITE);
		DrawTexture(ved[vedaet ? (int)(GetTime() * 5.0f) % 2 : 0], 488, 329, WHITE);

		EndDrawing();
	}

	if (caulkInit)
		caulk_Shutdown();

	CloseAudioDevice();
	CloseWindow();

	return EXIT_SUCCESS;
}
