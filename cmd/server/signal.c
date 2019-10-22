#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef linux
#include <signal.h>
#include <wait.h>
#endif

int g_sockfd;

void handle_child(int sig) {
	(void)sig;
	wait(NULL);
}

void handle_sigint(int sig) {
	(void)sig;
	puts("Cleaning up");
	printf("%d\n", g_sockfd);
	close(g_sockfd);
	exit(0);
}