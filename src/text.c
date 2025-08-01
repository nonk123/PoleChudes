#include <string.h>

#include "raylib.h"

#include "text.h"

extern Font font;
#define FONT_SIZE (16)
#define CELL_SIZE (21)

static char lines[MAX_LINES][LINE_MAX] = {0};

static float vedaUntil = 0.0;
static char veda[VEDA_MAX] = {0};

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

void textUpdate() {
	if (GetTime() >= vedaUntil)
		stopVeda();

	const int frame = 3;
	DrawRectangle(0, VEDA_Y, VEDA_WIDTH, VEDA_HEIGHT, BLACK);
	DrawRectangle(frame, VEDA_Y + frame, VEDA_WIDTH - 2 * frame + 1, VEDA_HEIGHT - 2 * frame, RAYWHITE);

	const float vedaHeight = 16.0, vedaWidth = 10.0, margin = 2.0;
	const int vedaLen = strlen(veda), reserved = 4, colMax = 24;
	int line = 0, col = 0;

	for (size_t i = 0; i < vedaLen; i++) {
		if (veda[i] == ' ') {
			if (colMax - col >= reserved)
				col++;
			else {
				line++;
				col = 0;
			}
		} else {
			Vector2 pos;
			pos.x = frame + margin + col * vedaWidth;
			pos.y = VEDA_Y + frame + margin + line * (vedaHeight + 2.0);
			DrawTextCodepoint(font, veda[i], pos, vedaHeight, BLACK);
			col++;
		}
	}

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

bool isVedaet() {
	return veda[0] != '\0';
}

void stopVeda() {
	veda[0] = '\0';
	vedaUntil = 0.0;
}

void vedaem(float duration, const char* text) {
	const size_t len = strlen(text);
	strncpy(veda, text, VEDA_MAX - 1);
	veda[len >= VEDA_MAX - 1 ? VEDA_MAX - 1 : len] = '\0';
	vedaUntil = GetTime() + duration;
}
