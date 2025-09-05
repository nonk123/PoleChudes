#include <string.h>

#include <S_tructures.h>

#include "gfx.h"

static StTinyMap* map = NULL;

#define PREFIX "assets/"
Texture gfx(const char* name) {
	static char fullPath[128] = {0};
	memcpy(fullPath, PREFIX, sizeof(PREFIX));
	memcpy(fullPath + sizeof(PREFIX) - 1, name, strlen(name));

	StTinyKey hash = 5381, c; // djb2 from: http://www.cse.yorku.ca/~oz/hash.html
	const char* ptr = fullPath;

	while ((c = (uint8_t)*ptr++))
		hash = (hash << 5) + hash + c;

	Texture* texPtr = NULL;
	if ((texPtr = StMapGet(map, hash)) != NULL)
		return *texPtr;

	Texture tex = LoadTexture(fullPath);
	StMapPut(map, hash, &tex, sizeof(tex));
	return tex;
}

void gfxInit() {
	map = NewTinyMap();
}

void gfxCleanup() {
	FreeTinyMap(map);
}
