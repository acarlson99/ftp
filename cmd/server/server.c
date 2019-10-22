#include "message.h"
#include "mysignal.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

int g_sockfd;

int config_socket(struct sockaddr_in *sock, int port) {
	g_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sockfd < 0) {
		perror("Unable to create socket");
		return (1);
	}

	bzero(sock, sizeof(*sock));
	sock->sin_family = AF_INET;
	sock->sin_addr.s_addr = htonl(INADDR_ANY);
	sock->sin_port = htons(port);
	int optval = 1;
	setsockopt(g_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

	if ((bind(g_sockfd, (struct sockaddr *)sock, sizeof(*sock))) != 0) {
		perror("Unable to bind socket");
		return (1);
	}
	printf("Bound socket\n");

	if ((listen(g_sockfd, 10)) != 0) {
		perror("Unable to listen");
		return (1);
	}
	return (0);
}

void handle_request(int connfd, t_request *req) {
	(void)req;
	t_response resp;
	resp.err = htons(0);
	resp.size = htons(0);
	// TODO: read file contents for PUT
	// TODO: make sure file opened properly
	/* int fd = open(req->filename, O_RDONLY); */
	int fd = open("Makefile", O_RDONLY);
	if (fd < 0) {
		// TODO: make errors mean something
		resp.err = htons(1);
		write(connfd, &resp, sizeof(resp));
	}
	char msgbuf[MAX_MSG_SIZE];
	int16_t size = 0;

	while ((size = read(fd, msgbuf, MAX_MSG_SIZE)) > 0) {
		printf("READ %u\n", size);
		resp.size = htons(size);
		printf("%x\n", resp.err);
		printf("WRITING TO CONN\n");
		write(connfd, &resp, sizeof(resp));
		printf("WRITING MSG\n");
		write(connfd, msgbuf, size);
	}
	resp.size = htons(0);
	write(connfd, &resp, sizeof(resp));
}

void handle_conn(int connfd) {
	char buf[256] = {0};
	ssize_t size;
	t_request request;
	while ((size = read(connfd, &request, sizeof(request))) > 0) {
		printf("%u %s\n", ntohs(request.cmd), request.filename);
		handle_request(connfd, &request);
		bzero(buf, sizeof(buf));
	}
	close(connfd);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: %s port\n", argv[0]);
		return (1);
	}

	int port = atoi(argv[1]);
	if (port < 1) {
		puts("Invalid port");
	}
	struct sockaddr_in servaddr, cli;

	signal(SIGINT, handle_sigint);
	signal(SIGTERM, handle_sigint);

	int bad = config_socket(&servaddr, port);
	if (bad) {
		close(g_sockfd);
		return (1);
	}

	printf("Listening on port %d\n", port);
	socklen_t len = sizeof(cli);
	int connfd;
	while ((connfd = accept(g_sockfd, (struct sockaddr *)&cli, &len))) {
		if (connfd < 0) {
			perror("Unable to accept connection");
			continue;
		}
		if (fork() == 0) {
			handle_conn(connfd);
			close(g_sockfd);
			exit(0);
		}
		signal(SIGCHLD, handle_child);
		close(connfd);
	}
}
