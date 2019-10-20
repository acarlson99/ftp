#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <signal.h>

#include <errno.h>

int g_sockfd;

void handle_child(int sig) {
	(void)sig;
	wait(NULL);
}

void handle_sigint(int sig) {
	(void)sig;
	puts("Cleaning up");
	close(g_sockfd);
	exit(0);
}

int config_socket(struct sockaddr_in *sock, int port) {
	g_sockfd = socket(AF_INET, SOCK_STREAM, 0);

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

void handle_conn(int connfd, int ii) {
	dprintf(connfd, "Connection %d\n", ii);
	char buf[256] = {0};
	while ((read(connfd, buf, 256)) > 0) {
		printf("%s", buf);
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
	if (bad)
		return (1);

	printf("Listening on port %d\n", port);
	socklen_t len = sizeof(cli);
	int connfd;
	int ii = 0;
	while ((connfd = accept(g_sockfd, (struct sockaddr *)&servaddr, &len))) {
		if (connfd < 0) {
			perror("Unable to accept connection");
			continue ;
		}
		if (fork() == 0) {
			sleep(1);
			handle_conn(connfd, ii);
			close(g_sockfd);
			exit(0);
		}
		signal(SIGCHLD, handle_child);
		close(connfd);
		ii++;
	}
}
