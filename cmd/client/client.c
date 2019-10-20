#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

int config_socket(struct sockaddr_in *sock, char *hostname, int port) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Unable to create socket");
		return (1);
	}
	bzero(sock, sizeof(*sock));

	sock->sin_family = AF_INET;
	sock->sin_addr.s_addr = inet_addr(hostname);
	sock->sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr *)sock, sizeof(*sock)) != 0) {
		perror("Unable to connect to socket");
		return (1);
	}
	return (0);
}

void handle_conn(int sockfd) {
	char *line = NULL;
	size_t line_cap = 0;
	while ((getline(&line, &line_cap, stdin)) != -1) {
		dprintf(sockfd, "%s", line);
	}
	free(line);
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("usage: %s hostname port\n", argv[0]);
		return (1);
	}

	char *hostname = argv[1];
	int port = atoi(argv[2]);
	if (port < 1) {
		puts("Invalid port");
	}

	struct sockaddr_in servaddr;
	int sockfd = config_socket(&servaddr, hostname, port);
	if (sockfd < 0) {
		return (1);
	}

	handle_conn(sockfd);
	close(sockfd);
}
