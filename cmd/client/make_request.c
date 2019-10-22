#include "client.h"
#include "fnv.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

void make_request(char *line, uint16_t reqcmd, t_request *req, int sockfd) {
	(void)line;
	req->cmd = htons(reqcmd);
	printf("REQ: %d %d\n", req->cmd, reqcmd);
	write(sockfd, req, sizeof(*req));
}

void handle_response(int sockfd, t_request *req) {
	struct s_response resp;
	(void)sockfd;
	/* read(sockfd, &resp, sizeof(resp)); */
	uint16_t err = 0;  // = ntohs(resp.err);
	uint16_t size = 0; // = ntohs(resp.err);
	if (err != 0) {
		// TODO: handle error
	}

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
	}
	else {
		fd = STDOUT_FILENO;
	}

	char msgbuf[MAX_MSG_SIZE];

	// TODO: read message
	while (1) {
		read(sockfd, &resp, sizeof(resp));
		printf("%x\n", resp.err);
		err = ntohs(resp.err);
		size = ntohs(resp.size);
		printf("RESP: %x %x\n", err, size);
		if (size <= 0 || err)
			break;
		read(sockfd, msgbuf, size);
		if (fd >= 0)
			write(fd, msgbuf, size);
	}

	if (fd > 2)
		close(fd);
}
