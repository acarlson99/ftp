#pragma once

#include "message.h"
#include <unistd.h>

size_t g_cmd_tab[10];
char *g_cmd_str[7];

void make_request(char *line, uint16_t reqcmd, t_request *req, int sockfd);
void listen_response(int sockfd);
