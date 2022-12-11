#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

#include "LineParser.h"


void execute(cmdLine *pCmdLine);


int main() {
	char promptBuffer[PATH_MAX];

	#define userCommandSize 20
	char userCommand[USR_CMD_SIZE];

	cmdLine *parsedUserCommand;

	while (1) {
		getcwd(promptBuffer, PATH_MAX);
		printf("%s> ", promptBuffer);

		fgets(userCommand, USR_CMD_SIZE, stdin);

		// printf("after fgets\n");

		if(feof(stdin)) {
			break;
		}

		parsedUserCommand = parseCmdLines(userCommand);

		// printf("before quit comarison\n");
		if (strncmp(parsedUserCommand->arguments[0], "quit", 5) == 0) {
			freeCmdLines(parsedUserCommand);
			break;
		}
		// printf("after quit comarison\n");

		// printf("executing %s\n", parsedUserCommand->arguments[0]);
		errno = 0;
		execute(parsedUserCommand);

		if (errno != 0) {
			perror("perror");
			freeCmdLines(parsedUserCommand);

			exit(EXIT_FAILURE);
		}

		// printf("after execute\n");

		freeCmdLines(parsedUserCommand);
		// printf("after freeCmdLines\n");
	}

	return 0;
}


void execute(cmdLine *pCmdLine) {
	pid_t pid;

	if((pid=fork()) == 0) {
		execvp(pCmdLine->arguments[0], pCmdLine->arguments);
		exit(EXIT_FAILURE);
	}

	waitpid(pid, NULL, 0);
	// printf("errno: %d\n", errno);
}


// echo sadfas