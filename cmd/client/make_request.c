#include "client.h"
#include "fnv.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

char *g_err_str[] = {
	[err_none] = "none",	   [err_unknowncmd] = "unknowncmd",
	[err_badfile] = "badfile", [err_baddir] = "baddir",
	[err_illegal] = "illegal", [err_fatal] = "fatal error",
};

// return 0 on success, 1 on minor err, 2 on fatal err
int handle_response(int sockfd, t_request *req)
{
	struct s_response resp;
	uint16_t err = 0;  // = ntohs(resp.err);
	uint16_t size = 0; // = ntohs(resp.err);

	int fd = -1;
	if (req->cmd == htons(cmd_get)) {
		char *filename = strrchr(req->filename, '/');
		if (!filename)
			filename = req->filename;
		else
			++filename;
		printf("Opening [%s]\n", filename);
		fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT | O_EXCL, S_IRWXU);
		if (fd < 0) {
			printf("Unable to open file [%s]\n", filename);
			perror("Unable to open file");
		}
	} else {
		fd = STDOUT_FILENO;
	}

	char msgbuf[MAX_MSG_SIZE];

	int ret = ERR_NONE;
	while (1) {
		if (read(sockfd, &resp, sizeof(resp)) < 0) {
			perror("Error reading header");
			ret = ERR_FATAL;
			break;
		}
		/* printf("%d\n", resp.err); */
		err = ntohs(resp.err);
		size = ntohs(resp.size);
		/* printf("RESP: %d %d\n", err, size); */
		if (err) {
			printf("Error\n");
			if (err < sizeof(g_err_str) / sizeof(*g_err_str)) {
				printf("ERR: %s\n", g_err_str[err]);
				if (err == err_fatal)
					exit(1);
			} else
				printf("ERR: Unknown error %d\n", err);
		}
		if (size <= 0)
			break;
		if (read(sockfd, msgbuf, size) < 0) {
			perror("Error reading body");
			ret = ERR_FATAL;
		}
		if (fd >= 0)
			write(fd, msgbuf, size);
	}

	if (fd > 2)
		close(fd);
	return (ret);
}

int make_put_request(char *line, t_request *req, int sockfd)
{
	struct s_response cli_resp;
	char msgbuf[MAX_MSG_SIZE] = {0};
	int fd = open(req->filename, O_RDONLY);
	int ret = ERR_NONE;

	(void)line;

	if (fd < 0) {
		perror("Unable to open file");
		ret = ERR_MINOR;
		goto end;
	}

	ssize_t size = 0;
	while ((size = read(fd, msgbuf, MAX_MSG_SIZE)) > 0) {
		cli_resp.size = htons(size);
		write(sockfd, &cli_resp, sizeof(cli_resp));
		write(sockfd, msgbuf, size);
	}
	if (size < 0) {
		perror("Unable to read from file");
		ret = ERR_MINOR;
		goto end;
	}

end:
	cli_resp.size = htons(0);
	write(sockfd, &cli_resp, sizeof(cli_resp));
	read(sockfd, &cli_resp, sizeof(cli_resp));
	return (ret);
}

// return 0 on success, 1 on minor err, 2 on fatal err
int make_request(char *line, uint16_t reqcmd, t_request *req, int sockfd)
{
	(void)line;
	req->cmd = htons(reqcmd);
	if ((reqcmd == cmd_get || reqcmd == cmd_put) && !*req->filename) {
		printf("ERR: get/put need filename argument\n");
		return (ERR_MINOR);
	}
	/* printf("REQ: %d %d\n", req->cmd, reqcmd); */
	if (write(sockfd, req, sizeof(*req)) < 0) {
		perror("Unable to write to socket");
		return (ERR_FATAL);
	}
	if (reqcmd == cmd_put) {
		return (make_put_request(line, req, sockfd));
	}

	return (handle_response(sockfd, req));
}
