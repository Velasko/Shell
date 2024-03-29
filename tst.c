/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include "readcmd.h"


int main()
{
	while (1) {
		struct cmdline *l;
		int i, j;

		printf("shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			printf("seq[%d]: ", i);
			for (j=0; cmd[j]!=0; j++) {
				printf("%s ", cmd[j]);
			}
			printf("\n");
		}
		int spid;
		switch(spid = fork()) {
	    case -1:
	      perror("fork");
				exit(-1);
	      break;
	    case 0 :
	      execvp(l->seq[0]);
	      break;
	    default:
	      execvp(l->seq[0], l->seq[1]);
				if(waitpid(spid, &status, 0) == -1) {
					perror("fork");
					exit(-1);
				}
	      break;
	  }
	}
}
