#include <string.h>

#include "raylib.h"

#include "text.h"

extern Font font;
#define FONT_SIZE (16)
#define CELL_SIZE (21)

static char lines[MAX_LINES][LINE_MAX] = {0};

void setLine(int line, const char* text) {
	if (line < 0 || line >= MAX_LINES)
		return;
	const size_t len = strlen(text);
	strncpy(lines[line], text, LINE_MAX);
	if (len < LINE_MAX)
		memset(lines[line] + len, ' ', LINE_MAX - len - 1);
}

void clearLines() {
	for (size_t i = 0; i < MAX_LINES; i++)
		setLine(i, "");
}

void drawTextLines() {
	for (size_t i = 0; i < MAX_LINES; i++) {
		const char* text = lines[i];
		const int y = 15 + i * (CELL_SIZE - 1);

		for (size_t j = 0; j < strlen(text); j++) {
			const int x = 120 + j * CELL_SIZE - j;
			DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, BLACK);
			DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, LIGHTGRAY);

			if (text[j] == ' ')
				continue;

			Vector2 pos = {.x = x + 6.5, .y = y + 3.0};
			DrawTextCodepoint(font, text[j], pos, FONT_SIZE, BLACK);
		}
	}
}

static bool vedaet = false;

bool isVedaet() {
	return vedaet;
}

void setVedaet(bool value) {
	vedaet = value;
}
