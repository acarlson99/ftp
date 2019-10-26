#include "client.h"
#include "fnv.h"
#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

char *g_cmd_str[8] = {
	[cmd_ls] = "ls",	 [cmd_cd] = "cd",   [cmd_pwd] = "pwd",
	[cmd_get] = "get",   [cmd_put] = "put", [cmd_quit] = "quit",
	[cmd_help] = "help", [cmd_none] = "",
};

char *get_cmd_type(char *line, uint16_t *reqcmd)
{
	*reqcmd = cmd_none;
	size_t end = strcspn(line, " \t\n");
	size_t hash = fnv_hashn(line, end);
	for (unsigned ii = 0; ii < sizeof(g_cmd_str) / sizeof(*g_cmd_str); ++ii) {
		if (hash == g_cmd_tab[ii]) {
			*reqcmd = ii;
			break;
		}
	}
	line += end;
	return (line + strspn(line, " \t"));
}

int config_socket(struct sockaddr_in *sock, char *server_ipv4, int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Unable to create socket");
		return (-1);
	}
	bzero(sock, sizeof(*sock));

	sock->sin_family = AF_INET;
	sock->sin_addr.s_addr = inet_addr(server_ipv4);
	sock->sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr *)sock, sizeof(*sock)) != 0) {
		perror("Unable to connect to socket");
		return (-1);
	}
	return (sockfd);
}

void exec_local(char *line)
{
	if (!strncmp(line, "cd ", 3)) {
		line += 3;
		line += strspn(line, " \t");
		chdir(line);
	} else {
		system(line);
	}
}

#define MAX(a, b) (a > b ? a : b)

void handle_conn(int sockfd)
{
	char *line = NULL;
	size_t line_cap = 0;
	ssize_t len = 0;
	for (;;) {
		write(1, "> ", 2);
		len = getline(&line, &line_cap, stdin);
		if (len == -1)
			break;
		line[len - 1] = '\0';
		char *working = line + strspn(line, " \t");

		// handle !ls etc.
		if (working[0] == '!') {
			exec_local(working + 1);
			continue;
		}

		t_request req;
		bzero(&req, sizeof(req));
		uint16_t reqcmd = 0;
		working = get_cmd_type(line, &reqcmd);

		size_t arg_len;
		// handle internal commands
		switch (reqcmd) {
		case cmd_quit:
			goto end;
			break;
		case cmd_help:
			printf("\
| command    | example        | summary                      |\n\
|------------|----------------|------------------------------|\n\
| `ls`       | `ls`           | list files in dir            |\n\
| `cd dir`   | `cd ../`       | change dir                   |\n\
| `pwd`      | `pwd`          | print working dir            |\n\
| `get file` | `get file.txt` | download file                |\n\
| `put file` | `put file.txt` | upload file                  |\n\
| `!command` | `!cd /tmp`     | run command on local machine |\n");
			break;
		case cmd_none:
			printf("Unrecognized command\n");
			break;
		default:
			arg_len = strcspn(line, "\n");
			strncpy(req.filename, working, MAX(arg_len, MAX_FILENAME_SIZE));
			if (make_request(working, reqcmd, &req, sockfd) == ERR_FATAL)
				goto end;
		}
	}

end:
	free(line);
}

#define HOSTBUF_SIZE 256

// TODO: make error printf print to stderr
int hostname_to_ipv4(char *hostname, char *buf)
{
	struct hostent *he;
	struct in_addr **addr_list;

	if ((he = gethostbyname(hostname)) == NULL) {
		herror("gethostbyname");
		return (1);
	}
	addr_list = (struct in_addr **)he->h_addr_list;
	if (!addr_list[0]) {
		printf("Address list index not found\n");
		return (1);
	}
	strncpy(buf, inet_ntoa(*addr_list[0]), HOSTBUF_SIZE);
	return (0);
}

void handle_sigpipe(int sig)
{
	(void)sig;
	printf("Interrupted connection\n");
	exit(1);
}

void usage(char *binname) { printf("usage: %s [-4n] port\n", binname); }

// ./client -n e1z3r2p2.42.us.org 8080
// ./client -4 127.0.0.1 8080
int main(int argc, char **argv)
{
	int ch;
	char *server_ipv4 = NULL;
	char hostbuf[HOSTBUF_SIZE] = {0};
	int port = 0;
	int ipv4set = 0;
	char *binname = argv[0];

	while ((ch = getopt(argc, argv, "hn:4:")) != -1) {
		switch (ch) {
		case '4':
			server_ipv4 = optarg;
			ipv4set = 1;
			break;
		case 'n':
			if (hostname_to_ipv4(optarg, hostbuf)) {
				return (1);
			} else {
				server_ipv4 = hostbuf;
				ipv4set = 1;
			}
			break;
		case 'h':
			usage(binname);
			printf("\t4 - ipv4 addr\n\tn - hostname\n");
			return (0);
		default:
			usage(binname);
			return (1);
		}
	}
	argc -= optind;
	argv += optind;
	if (!ipv4set) {
		printf("Must specify server hostname/ipv4\n");
		usage(binname);
		return (1);
	}

	if (argc != 1) {
		printf("Must specify port\n");
		usage(binname);
		return (1);
	}
	port = atoi(argv[0]);
	if (port < 1) {
		printf("Invalid port `%s`\n", argv[0]);
		usage(binname);
		return (1);
	}

	signal(SIGPIPE, handle_sigpipe);

	printf("Connecting to %s\n", server_ipv4);
	for (unsigned ii = 0; ii < sizeof(g_cmd_str) / sizeof(*g_cmd_str); ++ii)
		if (g_cmd_str[ii])
			g_cmd_tab[ii] = fnv_hash(g_cmd_str[ii]);

	struct sockaddr_in servaddr;
	int sockfd = config_socket(&servaddr, server_ipv4, port);
	if (sockfd < 0) {
		return (1);
	}

	handle_conn(sockfd);
	close(sockfd);
}
