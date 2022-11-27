/* // #include <fcntl.h>
// #include <dirent.h>
// #include <sys/types.h>
// #include <dirent.h>
// #include <sys/stat.h>
 */

#define O_DIRECTORY	00200000
#define O_RDONLY	     00
/* 
// https://syscalls32.paolostivanin.com/
//  */

#include "util.h"
#include <dirent.h>
#include <fcntl.h>
#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_getdents 141
#define NULL 0
/* // #define O_RDWR 2
// #define SYS_SEEK 19
// #define SEEK_SET 0
// #define SHIRA_OFFSET 0x291
// #define BUF_SIZE 1024 */

typedef struct ent
{
  int inode;
  int offset;

  short structSize;
  char fileName[256];
} ent;

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{   
	#define BUF_SIZE 2000
	int folderDescriptor = 0;
	long bytesRead = 0;
	char bytesArray[BUF_SIZE];
	ent *fileInfo = (ent*)bytesArray;
	char *bytesIterator = bytesArray;
	/* // struct linux_dirent *d;
	// char d_type; */

	folderDescriptor = system_call(SYS_OPEN, ".", O_RDONLY/* | O_DIRECTORY */, 0);
	if(folderDescriptor < 0) {
		system_call(SYS_WRITE, STDOUT, "error\n", 6);
	}

	bytesRead = system_call(SYS_getdents , folderDescriptor, bytesArray, BUF_SIZE);

	/* system_call(SYS_WRITE, STDOUT, itoa(bytesRead), 12 );
	system_call(SYS_WRITE, STDOUT, " bytes were read", 16 );
	system_call(SYS_WRITE,STDOUT,"\n",1); */

	char *bytesArrayEnd = bytesArray + bytesRead;
	for (bytesIterator = bytesArray; bytesIterator < bytesArrayEnd; bytesIterator += fileInfo->structSize) {
		fileInfo = (ent*)bytesIterator;

		if (strncmp(fileInfo->fileName, ".", 2) == 0 ||
			strncmp(fileInfo->fileName, "..", 3) == 0) {
			continue;
		}
		
		system_call(SYS_WRITE, STDOUT, fileInfo->fileName, strlen(fileInfo->fileName) );
		system_call(SYS_WRITE,STDOUT, "\n", 1);
	}

	system_call(SYS_CLOSE, folderDescriptor);
	return 0;
}
