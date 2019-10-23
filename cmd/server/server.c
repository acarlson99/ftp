#include "command.h"
#include "mysignal.h"
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

int g_sockfd;

int config_socket(struct sockaddr_in *sock, int port)
{
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

void (*cmd_ftab[])(int, t_response *, t_request *) = {
	[cmd_ls] = command_ls,   [cmd_cd] = command_cd,   [cmd_pwd] = command_pwd,
	[cmd_get] = command_get, [cmd_put] = command_put, [cmd_quit] = command_quit,
};

void handle_request(int connfd, t_request *req)
{
	t_response resp;
	resp.err = htons(err_none);
	resp.size = htons(0);
	uint16_t reqcmd = ntohs(req->cmd);
	if (reqcmd < 0 || reqcmd > sizeof(cmd_ftab) / sizeof(*cmd_ftab)
		|| !cmd_ftab[reqcmd]) {
		resp.err = htons(err_unknowncmd);
		goto end;
	}
	// Writes headers/bodies then sets error and size to be sent at end
	cmd_ftab[reqcmd](connfd, &resp, req);

end:
	write(connfd, &resp, sizeof(resp));
}

void handle_conn(int connfd)
{
	char buf[256] = {0};
	ssize_t size;
	t_request request;
	while ((size = read(connfd, &request, sizeof(request))) > 0) {
		printf("REQ: %u %s\n", ntohs(request.cmd), request.filename);
		handle_request(connfd, &request);
		bzero(buf, sizeof(buf));
	}
	close(connfd);
}

int main(int argc, char **argv)
{
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
		} else if (fork() == 0) {
			handle_conn(connfd);
			close(g_sockfd);
			exit(0);
		}
		signal(SIGCHLD, handle_child);
		close(connfd);
	}
}
