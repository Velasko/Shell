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

		int n;
		int pipecreationerror = 0;
		for (n=0; l->seq[n]!=0; n++);
		n -= 1;

		int pipes[n][2];
		for(int i = 0; i < n; i++){
			if(pipe(pipes[i]) == -1){
//				printf("Error on creating pipe\n");
				pipecreationerror = 1;
			}
//			printf("pipe: %d-%d\n", pipes[i][0], pipes[i][1]);
		}

		int WRITE = 1;
		int READ = 0;

		pid_t fils[n+1];

/*
		if (l->err) {

			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in*/

		/* If input stream closed, normal termination */
		if(1 - pipecreationerror){
			for (int i=0; l->seq[i]!=0; i++) {

				char **cmd = l->seq[i];

			if(strcmp(cmd[0], "exit") == 0){
				return(0);
			}

				p = fork();
				fils[i] = p;
				switch(p) {
					case -1:
					perror("fork");
					break;
					case 0 :
					//fils
					if(l->in && i == 0) {
						//First command with input file
						int fd = open(l->in, O_RDONLY, S_IRUSR);
						dup2(fd, STDIN_FILENO);
						close(fd);
					}else if(i != 0){
						//Not the first command
						dup2(pipes[i-1][READ], STDIN_FILENO);
						close(pipes[i-1][READ]);
						close(pipes[i-1][WRITE]);
					}

					if (l->out && l->seq[i+1]==0) {
						//Last command and a file as output
						//printf("out: %s\n", l->out);
						int fd = open(l->out, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
						dup2(fd, STDOUT_FILENO);
						close(fd);
					}else if(l->seq[i+1]!=0){
						//There are other commands waiting for this one's output
						dup2(pipes[i][WRITE], STDOUT_FILENO);
						close(pipes[i][WRITE]);
						close(pipes[i][READ]);
					}

					//Executing the command
					execvp(cmd[0], cmd);

					return(0);
					break;
				}

				//Dad's pipe closing process.
				if(i > 0){
					close(pipes[i-1][WRITE]);
					close(pipes[i-1][READ]);
				}


			}

			//Waiting for all the sons:
			for(int i = 0; i < n+1; i++){
				waitpid(fils[i], &status, 0);
			}
			sleep(0.7);
		}
	}
}