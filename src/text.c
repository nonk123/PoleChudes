#include <string.h>

#include "raylib.h"

#include "text.h"

extern Font font;
#define FONT_SIZE (16)
#define CELL_SIZE (18)

void writeLine(int line, const char* text) {
	const int y = 24 + line * (4 + CELL_SIZE) - line;

	for (size_t i = 0; i < strlen(text); i++) {
		if (text[i] == ' ')
			continue;

		const int x = 128 + i * CELL_SIZE - i;
		DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, BLACK);
		DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, LIGHTGRAY);

		Vector2 pos = {.x = x + 5.0, .y = y + 2.0};
		DrawTextCodepoint(font, text[i], pos, FONT_SIZE, BLACK);
	}
}
