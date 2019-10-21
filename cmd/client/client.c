#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "fnv.h"
#include "message.h"

int config_socket(struct sockaddr_in *sock, char *hostname, int port) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Unable to create socket");
		return (-1);
	}
	bzero(sock, sizeof(*sock));

	sock->sin_family = AF_INET;
	sock->sin_addr.s_addr = inet_addr(hostname);
	sock->sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr *)sock, sizeof(*sock)) != 0) {
		perror("Unable to connect to socket");
		return (-1);
	}
	return (sockfd);
}

size_t g_cmd_tab[10];
char *g_cmd_str[] = {
	[cmd_none] = "",	 [cmd_ls] = "ls",   [cmd_cd] = "cd",
	[cmd_get] = "get",   [cmd_put] = "put", [cmd_pwd] = "pwd",
	[cmd_quit] = "quit",
};

char *parse_cmd(char *line, t_request *req) {
	(void)line;
	req->cmd = cmd_none;

	size_t end = strcspn(line, " \t\n");
	size_t hash = fnv_hashn(line, end);

	for (unsigned ii = 0; ii < sizeof(g_cmd_str) / sizeof(*g_cmd_str); ++ii) {
		if (hash == g_cmd_tab[ii]) {
			req->cmd = ii;
			break;
		}
	}

	return (line + end);
}

void exec_local(char *line) {
	if (!strncmp(line, "cd ", 3)) {
		line += 3;
		line += strspn(line, " \t");
		chdir(line);
	}
	else {
		system(line);
	}
}

void make_request(char *line, int sockfd) {
	t_request req;
	bzero(&req, sizeof(req));
	parse_cmd(line, &req);
	write(sockfd, &req, sizeof(req));
}

void handle_conn(int sockfd) {
	char *line = NULL;
	size_t line_cap = 0;
	ssize_t len = 0;
	while ((len = getline(&line, &line_cap, stdin)) != -1) {
		line[len - 1] = '\0';
		char *working = line + strspn(line, " \t");
		if (working[0] == '!')
			exec_local(working + 1);
		else {
			make_request(working, sockfd);
		}
	}
	free(line);
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("usage: %s hostname port\n", argv[0]);
		return (1);
	}

	char *hostname;
	if (!strcmp(argv[1], "localhost"))
		hostname = "127.0.0.1";
	else
		hostname = argv[1];
	int port = atoi(argv[2]);
	if (port < 1) {
		puts("Invalid port");
		return (1);
	}

	for (unsigned ii = 0; ii < sizeof(g_cmd_str) / sizeof(*g_cmd_str); ++ii)
		if (g_cmd_str[ii])
			g_cmd_tab[ii] = fnv_hash(g_cmd_str[ii]);

	struct sockaddr_in servaddr;
	int sockfd = config_socket(&servaddr, hostname, port);
	if (sockfd < 0) {
		return (1);
	}

	handle_conn(sockfd);
	close(sockfd);
}
