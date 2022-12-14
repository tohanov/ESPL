#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

char checkDebugFlag(int argc, char* argv[]) {
	for (int i = 0; i < argc; ++i) {
		if (strncmp(argv[i], "-d", 3) == 0) { 
			return 1;
		}
	}

	return 0;
}


// void executeWithArgs() {

// }


int main(int argc, char* argv[]) {
	int pipeEnds[2]; // index 0 is read, 1 is write
	int readEnd = 0, writeEnd = 1;

	char* ls[] = {"ls", "-l", 0};
	char* tail[] = {"tail", "-n", "2", 0};
	pid_t pidChild1, pidChild2;

	char inDebugMode = checkDebugFlag(argc, argv);

	if (pipe(pipeEnds) == -1) {
		perror("pipe()");
		exit(EXIT_FAILURE);
	}

	if (inDebugMode) {
		fprintf(stderr, "(parent_process>forking…)\n");
	}

	if ((pidChild1 = fork()) == 0) { // first child
		if (inDebugMode) {
			fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n");
		}
		close(STDOUT_FILENO);
		dup(pipeEnds[writeEnd]);
		close(pipeEnds[writeEnd]);

		if (inDebugMode) {
			fprintf(stderr, "(child1>going to execute cmd: …)\n");
		}
		execvp(ls[0], ls);
		exit(EXIT_FAILURE);
	}
	else { // parent
		
		if (inDebugMode) {
			fprintf(stderr, "(parent_process>created process with id: %d)\n", pidChild1);
			fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");
		}
		
		close(pipeEnds[writeEnd]); // FIXME : uncomment
		
		if ((pidChild2 = fork()) == 0) { // second child
			if (inDebugMode) {
				fprintf(stderr, "(child2>redirecting stdout to the write end of the pipe…)\n");
			}
			close(STDIN_FILENO);
			dup(pipeEnds[readEnd]);
			close(pipeEnds[readEnd]);

			if (inDebugMode) {
				fprintf(stderr, "(child2>going to execute cmd: …)\n");
			}

			execvp(tail[0], tail);
			exit(EXIT_FAILURE);
		}
		else { // parent

			if (inDebugMode) {
				fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n");
			}
			
			close(pipeEnds[readEnd]);

			if (inDebugMode) {
				fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n");
			}
			
			waitpid(pidChild1, NULL, 0);
			waitpid(pidChild2, NULL, 0);

			if (inDebugMode) {
				fprintf(stderr, "(parent_process>exiting…)\n");
			}
		}

	}

	return 0;
}

