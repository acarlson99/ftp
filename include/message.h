#pragma once

#include <stdint.h>

enum e_cmd {
	cmd_ls,
	cmd_cd,
	cmd_get,
	cmd_put,
	cmd_pwd,
	cmd_quit,
};

struct s_message {
	uint8_t cmd;
	uint8_t args;
};

typedef struct s_message t_message;
typedef unsigned char t_arg[256];
