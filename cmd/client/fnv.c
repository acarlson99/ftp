#include "fnv.h"

size_t fnv_hash(char *input) {
	size_t hash;
	size_t ii;

	ii = 0;
	hash = FNV_OFFSET_BASIS;
	while (input[ii]) {
		hash = hash * FNV_PRIME;
		hash = hash ^ input[ii];
		++ii;
	}
	return (hash);
}

size_t fnv_hashn(char *input, size_t size) {
	size_t hash;
	size_t ii;

	ii = 0;
	hash = FNV_OFFSET_BASIS;
	while (input[ii] && ii < size) {
		hash = hash * FNV_PRIME;
		hash = hash ^ input[ii];
		++ii;
	}
	return (hash);
}
