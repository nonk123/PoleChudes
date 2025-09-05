#include <stdlib.h>

#include <nutpunch.h>
#include <raylib.h>

int main(int argc, char* argv[]) {
	InitWindow(800, 600, "Pole Chudes");
	InitAudioDevice();

	SetTargetFPS(60);
	SetExitKey(KEY_ESCAPE);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		EndDrawing();
	}

	CloseAudioDevice();
	CloseWindow();
	return EXIT_SUCCESS;
}
