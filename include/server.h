#pragma once

#define G_PATH_DEPTH 64
#define G_PATH_LENGTH 64

#include <unistd.h>

char *g_home_dir;
size_t g_home_len;
char g_path[G_PATH_DEPTH][G_PATH_LENGTH];
int g_path_idx;
