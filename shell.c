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
  pid_t p=0;
	while (1) {
		struct cmdline *l;


		printf("shell> ");
		l = readcmd();

		int pipein;
		int pipeout;
		int nextinpipe = 0;


/*
		if (l->err) {

			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in*/

		/* If input stream closed, normal termination */
		for (int i=0; l->seq[i]!=0; i++) {
			pipein = nextinpipe;
//			printf("%d\n", pipein);

			if(l->seq[i+1]!=0){
				int fd[2];
				if(pipe(fd) == -1)
					printf("Error on creating pipe\n");
				pipeout = fd[1];
				nextinpipe = fd[0];
			}


			char **cmd = l->seq[i];
			if(strcmp(cmd[0], "exit") == 0){
				return(0);
			}

			pid_t p = fork();
			switch(p) {
			case -1:
				perror("fork");
				break;
			case 0 :
			//fils
				if (l->out && l->seq[i+1]==0) {
					//Last command and a file as output
					//printf("out: %s\n", l->out);
					int fd = open(l->out, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
					dup2(fd, STDOUT_FILENO);
					close(fd);
				}else if(l->seq[i+1]!=0){
					//There are other commands waiting for this one's output
					dup2(pipeout, STDOUT_FILENO);
					close(pipeout);
				}

				if(l->in && i == 0) {
					//First command with input file
					int fd = open(l->in, O_RDONLY, S_IRUSR);
					dup2(fd, STDIN_FILENO);
					close(fd);
				}else if(i != 0){
					//Not the first command
					dup2(pipein, STDIN_FILENO);
					close(pipein);
				}

				execvp(cmd[0], cmd);
				
//				if(l->seq[i+1]==0) close(pipeout);
//				if(i != 0) close(pipein);

				return(0);
				break;
			}
		}
	waitpid(p, &status, 0);
	sleep(0.7);
	}
}
