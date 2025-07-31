#include <string.h>

#include "raylib.h"

#include "text.h"

extern Font font;
#define FSIZE (18)

void writeLine(int line, const char* text) {
	const int y = 24 + line * FSIZE;

	for (size_t i = 0; i < strlen(text); i++) {
		if (text[i] == ' ')
			continue;

		const int x = 128 + i * FSIZE;
		DrawRectangle(x, y, FSIZE, FSIZE, BLACK);
		DrawRectangle(x + 1, y + 1, FSIZE - 2, FSIZE - 2, LIGHTGRAY);

		Vector2 pos = {.x = x + 5.0, .y = y};
		DrawTextCodepoint(font, text[i], pos, FSIZE, BLACK);
	}
}
