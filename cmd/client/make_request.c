#include "client.h"
#include "fnv.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

// return 0 on success, 1 on minor err, 2 on fatal err
int handle_response(int sockfd, t_request *req)
{
	struct s_response resp;
	(void)sockfd;
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
		printf("%d\n", resp.err);
		err = ntohs(resp.err);
		size = ntohs(resp.size);
		printf("RESP: %d %d\n", err, size);
		if (err) {
			printf("ERR: %d\n", err);
		}
		if (size <= 0)
			break;
		if (read(sockfd, msgbuf, size) < 0) {
			perror("Error reading header");
			ret = ERR_FATAL;
		}
		if (fd >= 0)
			write(fd, msgbuf, size);
	}

	if (fd > 2)
		close(fd);
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
	printf("REQ: %d %d\n", req->cmd, reqcmd);
	if (write(sockfd, req, sizeof(*req)) < 0) {
		perror("Unable to write to socket");
		return (ERR_FATAL);
	}

	return (handle_response(sockfd, req));
}
