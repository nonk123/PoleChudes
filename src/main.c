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
	GSM_WAITING,
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

static bool gameRunning = true;
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
	SetExitKey(KEY_NULL);

	gfxInit();

	while (!WindowShouldClose() && gameRunning) {
		BeginDrawing();
		ClearBackground(BLACK);
		if (gsm[gameState].tick != NULL)
			gsm[gameState].tick();
		else if (IsKeyPressed(KEY_ESCAPE))
			break;
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

static void drawPole() {
	Color bg = {170, 170, 170, 255};
	ClearBackground(bg);
	DrawTexture(gfx("fon_pole.png"), 0, 0, WHITE);
}

enum {
	MN_MAIN,
	MN_ONLINE,
	MN_HOST,
	MN_JOIN,
	MN_SIZE,
};

static void goSingle(), goOnline(), fuckingExit(), findLobby(), hostLobby(), goHost();
static void noop() {}

#define OPTION_SIZE (64)
#define MAX_OPTIONS (16)
struct MenuOption {
	char display[OPTION_SIZE];
	void (*click)();
};
struct MenuOption menus[MN_SIZE][MAX_OPTIONS] = {
	[MN_MAIN] = {{"Singleplayer", goSingle}, {"Online", goOnline}, {"Exit", fuckingExit},},
	[MN_ONLINE] = {{"Find a lobby", findLobby}, {"Host your own", hostLobby}},
	[MN_HOST] = {{"Players: FMT", noop}, {"Start!", goHost}},
	[MN_JOIN] = {}, // populated after lobby list refresh
};
int menu = MN_MAIN, menuFrom[MN_SIZE] = {0};

static int numOptions() {
	for (int i = 0; i < MAX_OPTIONS; i++)
		if (menus[menu][i].click == NULL)
			return i;
	return MAX_OPTIONS;
}

void setMenu(int newMenu) {
	menuFrom[newMenu] = menu;
	menu = newMenu;
}

static void puke(const char* text, int x, int y, int fs) {
	static Font font = {0}, nullfont = {0};
	if (!memcmp(&font, &nullfont, sizeof(font)))
		font = LoadFont("assets/fnt/dos.ttf");
	Vector2 v;
	v.x = x;
	v.y = y;
	DrawTextEx(font, text, v, fs, 0.f, WHITE);
}

static void tickMainMenu() {
	drawPole();

	static int options[MN_SIZE] = {0};
	if (IsKeyPressed(KEY_DOWN))
		options[menu]++;
	if (IsKeyPressed(KEY_UP))
		options[menu]--;
	if (options[menu] < 0)
		options[menu] = numOptions() - 1;
	if (options[menu] >= numOptions())
		options[menu] = 0;

	if (numOptions() && IsKeyPressed(KEY_SPACE))
		menus[menu][options[menu]].click();
	if (IsKeyPressed(KEY_ESCAPE)) {
		if (menu == MN_MAIN)
			fuckingExit();
		else
			menu = menuFrom[menu];
	}

	int x = 120, sy = 120, fs = 32;
	for (int i = 0; i < numOptions(); i++)
		puke(menus[menu][i].display, x, sy + i * fs, fs);
	if (numOptions())
		puke(">", x - fs, sy + options[menu] * fs, fs);
}

static void goSingle() {
	setState(GSM_POLE_CHUDES);
}

static void goOnline() {
	setMenu(MN_ONLINE);
}

static void findLobby() {
	setMenu(MN_JOIN);
}

static void hostLobby() {
	setMenu(MN_HOST);
}

static void goHost() {
	setState(GSM_WAITING);
}

static void fuckingExit() {
	gameRunning = false;
}
