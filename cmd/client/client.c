#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>

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

char *parse_cmd(char *line, t_request *req) {
	(void)line;
	req->cmd = cmd_ls;
	return (line);
}

void handle_conn(int sockfd) {
	char *line = NULL;
	size_t line_cap = 0;
	ssize_t len = 0;
	while ((len = getline(&line, &line_cap, stdin)) != -1) {
		t_request req;
		bzero(&req, sizeof(req));
		char *working = parse_cmd(line, &req);
		printf("%s\n", working);
		write(sockfd, &req, sizeof(req));
		/* dprintf(sockfd, "%s", line); */
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

	struct sockaddr_in servaddr;
	int sockfd = config_socket(&servaddr, hostname, port);
	if (sockfd < 0) {
		return (1);
	}

	handle_conn(sockfd);
	close(sockfd);
}
