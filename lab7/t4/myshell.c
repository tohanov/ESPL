#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "LineParser.h"


#define TERMINATED	-1
#define SUSPENDED	0
#define RUNNING 	1

char* statuses[] = {"Terminated", "Suspended", "Running"};

typedef struct process {
	cmdLine* cmd; /* the parsed command line*/
	pid_t pid; /* the process id that is running the command*/
	int status; /* status of the process: RUNNING/SUSPENDED/TERMINATED */
	struct process *next; /* next process in chain */
} process;


void execute(cmdLine *pCmdLine, process **processList);
char checkDebugFlag(int argc, char* argv[]);
char shellCommandExecuted(cmdLine *parsedUserCommand, process **processList);

void freeListNode(process *listNode);
void printProcessList(process **process_list);
void addProcess(process **process_list, cmdLine *cmd, pid_t pid);
void freeProcessList(process* process_list);
void updateProcessList(process **process_list);
void updateProcessStatus(process *process_list, int pid, int status);


#define USR_CMD_SIZE 2048
char inDebugMode;
char shouldQuit = 0;
FILE *debugStream;


// #define HISTLEN 20
#define HISTLEN 5
char* history[HISTLEN];
int newest = -1;
int oldest = 0;

#define TRUE (char)1
#define FALSE (char)0


void addToHistory(char* unparsedCommand) {
	cmdLine *tempParse = parseCmdLines(unparsedCommand);

	if (tempParse == NULL) return;

	char isHistoryRetrievalCommand = (
		tempParse->argCount > 0
		&& tempParse->arguments[0][0] == '!'
	);

	freeCmdLines(tempParse);

	if (isHistoryRetrievalCommand) return;

	char *copiedCommand = malloc(strlen(unparsedCommand) * sizeof(char) + 1);
	strcpy(copiedCommand, unparsedCommand);

	static char queueIsFull = 0;

	if (newest == HISTLEN - 1 && oldest == 0) {
		queueIsFull = 1;
	}

	newest = (newest + 1) % HISTLEN;
	history[newest] = copiedCommand;

	if (queueIsFull) {
		oldest = (oldest + 1) % HISTLEN;
	}
}


void printHistory() {
	int representedIndex = 0;

	for (int trueIndex = oldest; trueIndex != newest; 
			trueIndex = (trueIndex + 1) % HISTLEN, ++representedIndex) {
			printf("%d %s", representedIndex + 1, history[trueIndex]);
		}

	printf("%d %s", representedIndex + 1, history[newest]);
}


void freeHistory() {
	for (int trueIndex = oldest; trueIndex != newest; 
			trueIndex = (trueIndex + 1) % HISTLEN) {
			free(history[trueIndex]);
		}

	free(history[newest]);
}


char *getLatestNonHistoryCommand() {
	// printf("before for\n");
	if (newest == -1) return NULL;

	for (int iterator = newest; iterator != oldest; iterator = (iterator - 1) % HISTLEN) {
		cmdLine *tempParse = parseCmdLines(history[iterator]);

		char isHistory = (strncmp(tempParse->arguments[0], "history", 8) == 0);

		freeCmdLines(tempParse);

		if ( ! isHistory) {
			return history[iterator];
		}
	}

	// printf("after for\n");
	return history[oldest];
}


char *getNthCommand(int n) {
	// printf("before for\n");

	n -= 1; // adjusting for 0-based indexing

	if (newest == -1 || n < 0 || n > HISTLEN - 1 || (oldest <= newest && n > newest - oldest)) {
		puts("Error: out of range or no such entry yet");
		return NULL;
	}

	return history[(oldest + n) % HISTLEN];

	// if (newest < oldest) return history[(oldest + n) % HISTLEN];
	// else if (n < newest - oldest) return (oldest + n);
	// else {
	// 	puts("Error: out of range or no such entry yet");
	// 	return NULL;
	// }
}


void freeEverything(process **processList) {
	freeProcessList(*processList);
	free(processList);
	freeHistory();
}


void handleCommand(char *unparsedUserCommand, process **processList) {
	addToHistory(unparsedUserCommand);

	cmdLine *parsedUserCommand = parseCmdLines(unparsedUserCommand);

	if (parsedUserCommand == NULL) return;

	errno = 0;

	if (shellCommandExecuted(parsedUserCommand, processList)) {
		freeCmdLines(parsedUserCommand);
	}
	else {
		execute(parsedUserCommand, processList);
	}

	if (errno != 0) {
		perror("main()");
		freeEverything(processList);

		exit(EXIT_FAILURE);
	}
}


int main(int argc, char* argv[]) {
	// printf("%p\n", parseCmdLines(NULL));

	inDebugMode = checkDebugFlag(argc, argv);
	debugStream = stderr;

	char promptBuffer[PATH_MAX];
	char userCommand[USR_CMD_SIZE];

	process **processList = malloc(sizeof(process *));
	*processList = NULL;

	while ( ! shouldQuit) {
		getcwd(promptBuffer, PATH_MAX);
		printf("%s> ", promptBuffer);

		fgets(userCommand, USR_CMD_SIZE, stdin);

		if (feof(stdin)) break;

		handleCommand(userCommand, processList);
	}

	freeEverything(processList);
	return 0;
}


char shellCommandExecuted(cmdLine *parsedUserCommand, process **processList) {
	char *command = parsedUserCommand->arguments[0];

	if (command[0] == '!' && command[1] != '!') {
		int historyRepresentedIndex = atoi(command + 1);
		// printf("historyRepresentedIndex = %d\n", historyRepresentedIndex);
		handleCommand(getNthCommand(historyRepresentedIndex), processList);
		return TRUE;
	}
	
	char* builtinCommands[] = {
		"cd",
		"procs",
		"quit",
		"kill",
		"suspend",
		"wake",
		"history",
		"!!",
	};

	typedef enum builtinCommandType {
		CD = 0,
		PROCS,
		QUIT,
		KILL,
		SUSPEND,
		WAKE,
		HISTORY,
		LASTCOMMAND,
	} builtinCommandType;

	// identify
	builtinCommandType identifiedCommand = -1;
	for (builtinCommandType type = CD; type < sizeof(builtinCommands)/sizeof(char*); ++type) {
		int commandLength = strlen(builtinCommands[type]);

		if (strncmp(command, builtinCommands[type], commandLength) == 0) {
			identifiedCommand = type;
		}
	}

	// execute
	pid_t pidArg = -1;
	switch (identifiedCommand) {
	case CD:
		chdir(parsedUserCommand->arguments[1]);
		break;
	case PROCS:
		printProcessList(processList);
		break;
	case QUIT:
		shouldQuit = 1;
		break;
	case KILL:
		pidArg = atoi(parsedUserCommand->arguments[1]);
		kill(pidArg, SIGINT);
		break;
	case SUSPEND:
		pidArg = atoi(parsedUserCommand->arguments[1]);
		kill(pidArg, SIGTSTP);
		break;
	case WAKE:
		pidArg = atoi(parsedUserCommand->arguments[1]);
		kill(pidArg, SIGCONT);
		break;
	case HISTORY:
		printHistory();
		break;
	case LASTCOMMAND:
		handleCommand(getLatestNonHistoryCommand(), processList);
		break;
	default:
		break;
	}

	return identifiedCommand != -1;
}


char checkDebugFlag(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
		if (strncmp(argv[i], "-d", 3) == 0) {
			return TRUE;
		}
	}

	return FALSE;
}


void printArgv(int argc, char* const argv[], FILE *stream) {
	for (int i = 0; i < argc; ++i) {
		fprintf(stream, "%s ", argv[i]);
	}
}


#define WRITE_END 1
#define READ_END 0


void execute(cmdLine *pCmdLine, process **processList) {
	pid_t pidChild1, pidChild2;
	static int pipeEnds[2];

	if (pCmdLine->next != NULL) {
		if (pipe(pipeEnds) == -1) {
			perror("pipe()");
		}
	}

	if (pCmdLine->next == NULL) { // no pipe
		if((pidChild1=fork()) == 0) {
			if (pCmdLine->inputRedirect != NULL) {
				int newStdin = open(pCmdLine->inputRedirect, O_RDONLY);
				dup2(newStdin, STDIN_FILENO);
			}
			if (pCmdLine->outputRedirect != NULL) {
				int newStdout = open(pCmdLine->outputRedirect, O_RDWR | O_CREAT, 0777);
				dup2(newStdout, STDOUT_FILENO);
			}

			execvp(pCmdLine->arguments[0], pCmdLine->arguments);
			exit(EXIT_FAILURE);
		}
		else {
			addProcess(processList, pCmdLine, pidChild1);

			if (inDebugMode) {
				fprintf(debugStream, "PID: %d :: executing: ", pidChild1);
				printArgv(pCmdLine->argCount, pCmdLine->arguments, debugStream);
				fputc('\n', debugStream);
			}
		}

		if (pCmdLine->blocking) {
			waitpid(pidChild1, NULL, 0);
		}
	}
	else { // with pipe
		if ((pidChild1 = fork()) == 0) { // first child
			if (pCmdLine->inputRedirect != NULL) {
				int newStdin = open(pCmdLine->inputRedirect, O_RDONLY);
				dup2(newStdin, STDIN_FILENO);
			}
			// if (pCmdLine->outputRedirect != NULL) {
			// 	int newStdout = open(pCmdLine->outputRedirect, O_RDWR | O_CREAT, 0777);
			// 	dup2(newStdout, STDOUT_FILENO);
			// }

			close(STDOUT_FILENO);
			dup(pipeEnds[WRITE_END]);
			close(pipeEnds[WRITE_END]);

			execvp(pCmdLine->arguments[0], pCmdLine->arguments);
			exit(EXIT_FAILURE);
		}
		else { // parent
			
			close(pipeEnds[WRITE_END]); // FIXME : uncomment
			
			if ((pidChild2 = fork()) == 0) { // second child

				// if (pCmdLine->inputRedirect != NULL) {
				// 	int newStdin = open(pCmdLine->inputRedirect, O_RDONLY);
				// 	dup2(newStdin, STDIN_FILENO);
				// }

				pCmdLine = pCmdLine->next;

				if (pCmdLine->outputRedirect != NULL) {
					int newStdout = open(pCmdLine->outputRedirect, O_RDWR | O_CREAT, 0777);
					dup2(newStdout, STDOUT_FILENO);
				}

				close(STDIN_FILENO);
				dup(pipeEnds[READ_END]);
				close(pipeEnds[READ_END]);

				execvp(pCmdLine->arguments[0], pCmdLine->arguments);
				exit(EXIT_FAILURE);
			}
			else { // parent
				addProcess(processList, pCmdLine, pidChild1);
				// addProcess(processList, pCmdLine->next, pidChild2);

				close(pipeEnds[READ_END]);

				if (inDebugMode) {
					fprintf(debugStream, "PID: %d :: executing: ", pidChild1);
					printArgv(pCmdLine->argCount, pCmdLine->arguments, debugStream);
					fputc('\n', debugStream);
					
					fprintf(debugStream, "PID: %d :: executing: ", pidChild2);
					printArgv(pCmdLine->next->argCount, pCmdLine->next->arguments, debugStream);
					fputc('\n', debugStream);
				}
				
				if (pCmdLine->blocking) {
					waitpid(pidChild1, NULL, 0);
					waitpid(pidChild2, NULL, 0);
				}
			}

		}
	}

	// printf("errno: %d\n", errno);
}

// ================ task2
void addProcess(process **process_list, cmdLine *cmd, pid_t pid) {
	process *newProcess = malloc(sizeof(process));

	newProcess->cmd = cmd;
	newProcess->pid = pid;
	newProcess->status = RUNNING;
	newProcess->next = *process_list;

	*process_list = newProcess;
}


void deleteTerminated(process **processList) {
	process *iterator = *processList;

	while (iterator != NULL && iterator->status == TERMINATED) {
		*processList = iterator->next;
		freeListNode(iterator);
		iterator = *processList;
	}

	process *forwardPeek;

	while (iterator != NULL) {
		forwardPeek = iterator->next;

		if (forwardPeek != NULL && forwardPeek->status == TERMINATED) {
			iterator->next = forwardPeek->next;
			freeListNode(forwardPeek);
		}

		iterator = iterator->next;
	}
}


char *statusToString(int status) {
	return statuses[status + 1];
}


void printProcessList(process **process_list) {
	process *iterator = *process_list;

	updateProcessList(process_list);

	printf("%-15s%-15s%-15s\n", "PID", "STATUS", "Command");

	while (iterator != NULL) {
		printf("%-15d", iterator->pid);
		printf("%-15s", statusToString(iterator->status));
		printArgv(iterator->cmd->argCount, iterator->cmd->arguments, stdout);
		putchar('\n');

		iterator = iterator->next;
	}

	deleteTerminated(process_list);
}


void freeListNode(process *listNode) {
	freeCmdLines(listNode->cmd);
	free(listNode);
}


void freeProcessList(process *process_list) {
	process *iterator = process_list;
	
	while(iterator != NULL) {
		process *temp = iterator->next;

		freeListNode(iterator);

		iterator = temp;
	}
}


void updateProcessStatus(process *process_list, int pid, int status) {
	process *iterator = process_list;

	while (iterator != NULL) {
		if (iterator->pid == pid) {
			iterator->status = status;
			return;
		}
	}
}


void updateProcessList(process **process_list) {
	process *iterator = *process_list;
	int waitStatus;
	int waitReturn;

	while(iterator != NULL) {
		waitReturn = waitpid(iterator->pid, &waitStatus, WNOHANG | WCONTINUED | WUNTRACED);

		// if child state has changed, returns child pid
		// if not changed state, returns 0
		// if error, returns -1

		// printf("ret = %d\n", waitReturn);

		if (waitReturn == iterator->pid) { // status is available
			if (WIFSTOPPED(waitStatus)) {
				iterator->status = SUSPENDED;
			}
			else if (WIFCONTINUED(waitStatus)) {
				iterator->status = RUNNING;
			}
			else {
				iterator->status = TERMINATED;
			}
		}
		else if (waitReturn == -1) { // error
			// perror("updateProcessList()");
			iterator->status = TERMINATED;
		}

		iterator = iterator->next;
	}

	errno = 0;
}
// ================ end task2