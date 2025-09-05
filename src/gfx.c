#include <S_tructures.h>

#include "gfx.h"
#include "util.h"

static StTinyMap* map = NULL;

Texture gfx(const char* name) {
	name = asspath(name);
	StTinyKey hash = djb2(name);

	Texture* texPtr = NULL;
	if ((texPtr = StMapGet(map, hash)) != NULL)
		return *texPtr;

	Texture tex = LoadTexture(name);
	StMapPut(map, hash, &tex, sizeof(tex));
	return tex;
}

void gfxInit() {
	map = NewTinyMap();
}

void gfxCleanup() {
	FreeTinyMap(map);
}
