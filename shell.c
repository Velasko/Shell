/*
 * Copyright (C) 2002, Simon Nieuviarts
 */
#include <unistd.h>
#include <stdio.h>
#include "readcmd.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
  int status;
	while (1) {
		struct cmdline *l;


		printf("shell> ");
		l = readcmd();

    char **cmd = l->seq[0];
/*
		if (l->err) {

			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in*/

		/* If input stream closed, normal termination */
		pid_t p = fork();
		switch(p) {
    case -1:
      perror("fork");
      break;
    case 0 :
		//fils
			if (l->out) {
				//printf("out: %s\n", l->out);
				int fd = open(l->out,O_CREAT| O_TRUNC|  O_RDWR,S_IRWXU );
				dup2(fd, STDOUT_FILENO);
				close(fd);
			}
      execvp(cmd[0],cmd);
      break;
    default:
		//pid_t waitpid(pid_t pid, int *status, int options);
      waitpid(p, &status, 0);
      break;
    }
	}
}
