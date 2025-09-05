#include <string.h>

#include "util.h"

// From: <http://www.cse.yorku.ca/~oz/hash.html>
StTinyKey djb2(const char* s) {
	StTinyKey hash = 5381, c;
	while ((c = (uint8_t)*s++))
		hash = (hash << 5) + hash + c;
	return hash;
}

const char* asspath(const char* filename) {
	static char buf[128] = {0};
	memcpy(buf, ASSET_ROOT, sizeof(ASSET_ROOT));
	memcpy(buf + sizeof(ASSET_ROOT) - 1, filename, strlen(filename));
	return buf;
}
