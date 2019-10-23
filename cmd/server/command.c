#include "message.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

void command_ls(int connfd, t_response *resp, t_request *req)
{
	(void)connfd;
	(void)resp;
	(void)req;
}

void command_cd(int connfd, t_response *resp, t_request *req)
{
	(void)connfd;
	(void)resp;
	(void)req;
}

void command_get(int connfd, t_response *resp, t_request *req)
{
	int fd = open(req->filename, O_RDONLY);
	if (fd < 0) {
		// TODO: make errors mean something
		resp->err = htons(1);
		perror("Unable to open file");
		/* write(connfd, resp, sizeof(*resp)); */
		return;
	}
	char msgbuf[MAX_MSG_SIZE];
	int16_t size = 0;

	printf("AAAAA\n");
	while ((size = read(fd, msgbuf, MAX_MSG_SIZE)) > 0) {
		printf("READ %u from file %s\n", size, req->filename);
		resp->size = htons(size);
		write(connfd, resp, sizeof(*resp));
		write(connfd, msgbuf, size);
	}
	close(fd);
	resp->size = htons(0);
	resp->err = htons(0);
	/* write(connfd, resp, sizeof(resp)); */
}

void command_put(int connfd, t_response *resp, t_request *req)
{
	(void)connfd;
	(void)resp;
	(void)req;
}

void command_pwd(int connfd, t_response *resp, t_request *req)
{
	(void)connfd;
	(void)resp;
	(void)req;
}

void command_quit(int connfd, t_response *resp, t_request *req)
{
	(void)connfd;
	(void)resp;
	(void)req;
}
