#pragma once

#include <stdint.h>

#define MAX_MSG_SIZE 1024

enum e_cmd {
	cmd_none,
	cmd_ls,
	cmd_cd,
	cmd_get,
	cmd_put,
	cmd_pwd,
	// Not server commands
	cmd_quit,
	cmd_help,
	cmd_exec,
};

#define MAX_FILENAME_SIZE 256

struct s_request {
	// get
	uint16_t cmd;
	// file
	char filename[MAX_FILENAME_SIZE];
};

struct s_response {
	// 0 if no error
	uint16_t err;
	// size of response. size = 0 if EOT
	uint16_t size;
};

typedef struct s_request t_request;
typedef struct s_response t_response;
