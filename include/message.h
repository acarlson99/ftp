#pragma once

#include <stdint.h>

enum e_cmd {
	cmd_none,
	cmd_ls,
	cmd_cd,
	cmd_get,
	cmd_put,
	cmd_pwd,
	cmd_quit,
};

struct s_request {
	// get
	uint8_t cmd;
	// file
	char filename[256];
};

struct s_response {
	// 0 if no error
	uint8_t err;
	// size of response. size = 0 if EOT
	uint8_t size;
};

typedef struct s_request t_request;
