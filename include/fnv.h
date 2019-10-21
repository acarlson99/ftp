#pragma once

#include <stdlib.h>

#define FNV_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3

size_t fnv_hash(char *input);
size_t fnv_hashn(char *input, size_t size);
