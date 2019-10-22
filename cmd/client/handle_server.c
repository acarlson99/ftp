#include "client.h"
#include "fnv.h"
#include <string.h>

void make_request(char *line, t_request *req, int sockfd) {
	(void)line;
	write(sockfd, req, sizeof(*req));
}

void listen_response(int sockfd) {
	struct s_response resp;
	(void)sockfd;
	return;
	do {
		/* read(sockfd, &resp, sizeof(resp)); */
	} while (resp.size > 0);
}
