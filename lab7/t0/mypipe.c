// #include <linux/limits.h>
// #include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
// #include <errno.h>
#include <sys/wait.h>
// #include <string.h>

int main() {
	// int pipe(int pipefd[2]);

	int pipefd[2];
	pid_t pid;
	char buf[6];
	
	if (pipe(pipefd) == -1) {
		perror("pipe()");
		exit(EXIT_FAILURE);
	}

	if ((pid = fork()) == -1) {
		perror("fork()");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {    /* Child reads from pipe */
		close(pipefd[0]);          /* Close unused read end */

		write(pipefd[1], "hello", 6);

		close(pipefd[1]);
		exit(EXIT_SUCCESS);
	}
	else {            /* Parent writes argv[1] to pipe */
		close(pipefd[1]);          /* Close unused write end */

		read(pipefd[0], &buf, 6);
		puts(buf);

		close(pipefd[0]);
		exit(EXIT_SUCCESS);
	}

	return 0;
}