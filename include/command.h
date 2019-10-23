#pragma once

#include "message.h"

void command_ls(int connfd, t_response *resp, t_request *req);
void command_cd(int connfd, t_response *resp, t_request *req);
void command_get(int connfd, t_response *resp, t_request *req);
void command_put(int connfd, t_response *resp, t_request *req);
void command_pwd(int connfd, t_response *resp, t_request *req);
void command_quit(int connfd, t_response *resp, t_request *req);
