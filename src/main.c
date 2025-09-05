#include <stdlib.h>
#include <string.h>

#include <S_tructures.h>
#include <nutpunch.h>
#include <raylib.h>

#include "gfx.h"
#include "util.h"

static void tickIntro(), tickMainMenu();

enum {
	GSM_INTRO,
	GSM_MAIN_MENU,
	GSM_HOST,
	GSM_JOIN,
	GSM_LOBBY,
	GSM_POLE_CHUDES,
	GSM_SIZE,
};

struct GameState {
	void (*tick)();
};
static int gameState = GSM_INTRO;
struct GameState gsm[GSM_SIZE] = {
	[GSM_INTRO] = {tickIntro},
	[GSM_MAIN_MENU] = {tickMainMenu},
};

static void setState(int newState) {
	gameState = newState;
}

int main(int argc, char* argv[]) {
	bool playIntro = true;
	for (int i = 1; i < argc; i++)
		if (!strcmp(argv[i], "--skip-intro"))
			playIntro = false;
	if (!playIntro)
		setState(GSM_MAIN_MENU);

	InitWindow(640, 480, "Pole Chudes");
	InitAudioDevice();

	SetTargetFPS(60);
	SetExitKey(KEY_ESCAPE);

	gfxInit();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		if (gsm[gameState].tick != NULL)
			gsm[gameState].tick();
		EndDrawing();
	}

	gfxCleanup();

	CloseAudioDevice();
	CloseWindow();
	return EXIT_SUCCESS;
}

static void tickIntro() {
	static const char* disclaimers[] = {"discl1.png", "discl2.png"};
	static int cur = 0;
	static Color color = WHITE;
	static float period = 5.f, trans = 1.f, elapsed = 0.f;

	ClearBackground(BLACK);
	color.a = 255;
	if (elapsed <= trans)
		color.a = (uint8_t)(255.f * elapsed / trans);
	else if (elapsed >= period - trans)
		color.a = (uint8_t)(255.f * (period - elapsed) / trans);

	DrawTexture(gfx(disclaimers[cur]), 0, 0, color);

	elapsed += GetFrameTime();
	if (elapsed >= period || IsKeyPressed(KEY_SPACE)) {
		cur++;
		elapsed = 0.f;
	}

	if (cur >= Length(disclaimers))
		setState(GSM_MAIN_MENU);
}

static void tickMainMenu() {
	DrawText("NOT IMPLEMENTED YET", 5, 5, 20, RAYWHITE);
}
