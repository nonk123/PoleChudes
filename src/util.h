#pragma once

#include <S_tructures.h>

#define Length(arr) (sizeof(arr) / sizeof(*arr))
#define ASSET_ROOT "assets/"

StTinyKey djb2(const char*);
const char* asspath(const char*);
