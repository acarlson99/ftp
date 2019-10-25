#include "message.h"
#include "server.h"
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// TODO: handle write bad return

void command_ls(int connfd, t_response *resp, t_request *req)
{
	char msgbuf[MAX_MSG_SIZE];
	DIR *dr = opendir(".");

	(void)req;

	printf("HOME: %s\n", g_home_dir);

	printf("%s\n", __FUNCTION__);

	if (!dr) {
		// TODO: handle error
		resp->err = htons(err_baddir);
		goto end;
	}

	struct dirent *de;
	while ((de = readdir(dr))) {
		printf("%s\n", de->d_name);
		strncpy(msgbuf, de->d_name, MAX_MSG_SIZE - 2);
		strcat(msgbuf, "\n");
		size_t size = strlen(msgbuf);
		resp->size = htons(size);
		write(connfd, resp, sizeof(*resp));
		write(connfd, msgbuf, size);
	}

end:
	closedir(dr);
	resp->size = htons(0);
}

void command_cd(int connfd, t_response *resp, t_request *req)
{
	printf("%s\n", __FUNCTION__);
	(void)connfd;

	// TODO: validate cd arg
	int err = chdir(req->filename);
	if (err) {
		perror("Unable to chdir");
		resp->err = htons(err_baddir);
	}
}

#include <stdlib.h>

void command_pwd(int connfd, t_response *resp, t_request *req)
{
	printf("%s\n", __FUNCTION__);
	(void)connfd;
	(void)resp;
	(void)req;
	system("pwd");
}

void command_get(int connfd, t_response *resp, t_request *req)
{
	printf("%s\n", __FUNCTION__);
	// TODO: validste get arg
	int fd = open(req->filename, O_RDONLY);
	if (fd < 0) {
		resp->err = htons(err_badfile);
		perror("Unable to open file");
		return;
	}
	printf("Opened %s\n", req->filename);
	char msgbuf[MAX_MSG_SIZE];
	int16_t size = 0;

	while ((size = read(fd, msgbuf, MAX_MSG_SIZE)) > 0) {
		printf("READ %u from file %s\n", size, req->filename);
		resp->size = htons(size);
		write(connfd, resp, sizeof(*resp));
		write(connfd, msgbuf, size);
	}
	if (size < 0) {
		perror("Unable to read from file");
		resp->err = htons(err_badfile);
	} else {
		resp->err = htons(err_none);
	}
	close(fd);
	resp->size = htons(0);
}

// reverse get
void command_put(int connfd, t_response *resp, t_request *req)
{
	printf("%s\n", __FUNCTION__);
	(void)connfd;
	(void)resp;
	(void)req;
}
