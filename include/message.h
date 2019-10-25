#pragma once

#include <stdint.h>

#define MAX_MSG_SIZE 1024

// NOTE: server commands smaller values to optimize jump table size
enum e_cmd {
	// Server commands
	cmd_ls,
	cmd_cd,
	cmd_pwd,
	cmd_get,
	cmd_put,
	// Not server commands
	cmd_quit,
	cmd_help,
	// Default value
	cmd_none,
};

enum e_resp_err {
	err_none,		// no error
	err_unknowncmd, // unknown command sent
	err_badfile,	// file not found or illegal
	err_baddir,		// dir not found or illegal
	err_illegal,	// cd outsize home
	err_fatal,		// fatal error.  Exiting
};

#define MAX_FILENAME_SIZE 256

struct s_request {
	// get
	uint16_t cmd;
	// file
	char filename[MAX_FILENAME_SIZE + 1];
};

struct s_response {
	// 0 if no error
	uint16_t err;
	// size of response. size = 0 if EOT
	uint16_t size;
};

typedef struct s_request t_request;
typedef struct s_response t_response;
