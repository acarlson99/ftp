#pragma once

#include "message.h"
#include <unistd.h>

#define ERR_NONE 0
#define ERR_MINOR 1
#define ERR_FATAL 2

char *g_cmd_str[8];
size_t g_cmd_tab[sizeof(g_cmd_str) / sizeof(*g_cmd_str)];

int make_request(char *line, uint16_t reqcmd, t_request *req, int sockfd);
int handle_response(int sockfd, t_request *req);
