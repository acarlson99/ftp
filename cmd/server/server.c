#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <signal.h>

#include <errno.h>

int sockfd;

void handle_sigint(int sig) {
	(void)sig;
	printf("Cleaning up\n");
	close(sockfd);
	exit(0);
}

void config_socket(struct sockaddr_in *socket, int port) {
	socket->sin_family = AF_INET;
	socket->sin_addr.s_addr = htonl(INADDR_ANY);
	socket->sin_port = htons(port);
}

void handle_conn(int connfd, int ii) {
	dprintf(connfd, "Connection %d\n", ii);
	close(connfd);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: %s port\n", argv[0]);
		return (1);
	}
	int port = atoi(argv[1]);
	struct sockaddr_in servaddr, cli;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	signal(SIGINT, handle_sigint);

	config_socket(&servaddr, port);

	if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
		perror("Unable to bind socket");
		return (1);
	}
	printf("Bound socket\n");

	if ((listen(sockfd, 10)) != 0) {
		perror("Unable to listen");
	}
	printf("Listening on port %d\n", port);
	socklen_t len = sizeof(cli);
	int connfd;
	int ii = 0;
	while ((connfd = accept(sockfd, (struct sockaddr *)&servaddr, &len))) {
		if (connfd < 0) {
			perror("Unable to accept connection");
			continue ;
		}
		if (fork() == 0) {
			sleep(1);
			handle_conn(connfd, ii);
			close(sockfd);
			exit(0);
		}
		close(connfd);
		ii++;
	}
	printf("Server\n");
}
