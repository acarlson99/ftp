#include "client.h"
#include "fnv.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

void make_request(char *line, uint16_t reqcmd, t_request *req, int sockfd) {
	(void)line;
	req->cmd = htons(reqcmd);
	printf("REQ: %d %d\n", req->cmd, reqcmd);
	write(sockfd, req, sizeof(*req));
}

void listen_response(int sockfd) {
	struct s_response resp;
	(void)sockfd;
	do {
		read(sockfd, &resp, sizeof(resp));
		printf("RESP: %d %d\n", ntohs(resp.err), ntohs(resp.size));
		// TODO: read message
	} while (resp.size > 0 && !resp.err);
}
