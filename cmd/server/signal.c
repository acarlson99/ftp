#include "server.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int g_sockfd;

void handle_child(int sig)
{
	(void)sig;
	printf("Destroying child process\n");
	wait(NULL);
}

void handle_sigint(int sig)
{
	(void)sig;
	puts("Cleaning up");
	printf("%d\n", g_sockfd);
	free(g_home_dir);
	close(g_sockfd);
	exit(0);
}
