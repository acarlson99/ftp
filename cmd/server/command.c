#include "message.h"
#include "server.h"
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO: handle ALL write bad return

void command_ls(int connfd, t_response *resp, t_request *req)
{
	char msgbuf[MAX_MSG_SIZE];
	DIR *dr = opendir(".");

	(void)req;

	if (!dr) {
		resp->err = htons(err_baddir);
		goto end;
	}

	struct dirent *de;
	while ((de = readdir(dr))) {
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

// TODO: make this command less atrocious
void command_cd(int connfd, t_response *resp, t_request *req)
{
	(void)connfd;

	char *oldwd = getcwd(NULL, 0);

	int err = chdir(req->filename);
	if (err) {
		perror("Unable to chdir");
		resp->err = htons(err_baddir);
	}

	char *wd = getcwd(NULL, 0);
	if (strncmp(g_home_dir, wd, g_home_len - 1)) {
		resp->err = htons(err_baddir);
		if (!chdir(oldwd)) {
			perror("Unable to cd to old workingdir");
			if (!chdir(g_home_dir)) {
				perror("Unable to cd home");
				resp->err = htons(err_fatal);
			}
		}
	}

	free(oldwd);
	free(wd);
}

void command_pwd(int connfd, t_response *resp, t_request *req)
{
	char msgbuf[MAX_MSG_SIZE];
	(void)connfd;
	(void)resp;
	(void)req;

	bzero(msgbuf, MAX_MSG_SIZE);
	getcwd(msgbuf, MAX_MSG_SIZE);
	size_t size = strlen(msgbuf);
	if (size == g_home_len)
		msgbuf[size++] = '/';
	msgbuf[size++] = '\n';
	memmove(msgbuf, msgbuf + g_home_len, MAX_MSG_SIZE - g_home_len);

	size -= g_home_len;

	write(1, msgbuf, size);
	fflush(stdout);

	resp->size = htons(size);
	write(connfd, resp, sizeof(*resp));
	write(connfd, msgbuf, size);
	write(1, msgbuf, size);

	resp->size = htons(0);
	resp->err = htons(err_none);
}

// TODO: make this less bad
void command_get(int connfd, t_response *resp, t_request *req)
{
	char *filename = strrchr(req->filename, '/');
	if (!filename)
		filename = req->filename;
	else
		++filename;
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		resp->err = htons(err_badfile);
		perror("Unable to open file");
		return;
	}
	char msgbuf[MAX_MSG_SIZE];
	int16_t size = 0;

	while ((size = read(fd, msgbuf, MAX_MSG_SIZE)) > 0) {
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
	struct s_response cli_resp;
	uint16_t err = 0;
	uint16_t size = 0;

	int fd = -1;
	char *filename = strrchr(req->filename, '/');
	if (!filename)
		filename = req->filename;
	else
		++filename;
	fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
	if (fd < 0) {
		printf("Unable to open file [%s]\n", filename);
		perror("Unable to open file");
	}

	char msgbuf[MAX_MSG_SIZE];

	while (1) {
		if (read(connfd, &cli_resp, sizeof(cli_resp)) < 0) {
			perror("Error reading header");
			break;
		}
		err = ntohs(cli_resp.err);
		size = ntohs(cli_resp.size);
		if (size <= 0)
			break;
		if (read(connfd, msgbuf, size) < 0) {
			perror("Error reading header");
		}
		if (fd >= 0)
			write(fd, msgbuf, size);
	}

	if (fd > 2)
		close(fd);
	resp->size = htons(0);
}
