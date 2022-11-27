/* // #include <fcntl.h>
// #include <dirent.h>
// #include <sys/types.h>
// #include <dirent.h>
// #include <sys/stat.h>
 */
extern system_call();

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
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291
#define BUF_SIZE 1024

#include <stdlib.h> /* FIXME : remove */
#include <stdio.h> /* FIXME : remove */

typedef struct ent
{
  int inode;
  int offset;
  short len;
  char buf[1];
}ent;

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{   
	int fd;
	long nread;
	char buf[2000];
	ent *entp = buf;
	struct linux_dirent *d;
	char d_type;
	fd = system_call(SYS_OPEN,".",O_RDONLY,0);


	if(fd<0)
		system_call(SYS_WRITE,STDOUT,"e\n",2);
	/* // for (;;) { */

	nread = system_call(141, fd, entp->buf, 100);

	entp = buf + 28;
/* // printf("Inode is %d, offset is %d, size is %d, name is %s\n",
	// 	entp->inode,
	// 	entp->offset,
	// 	entp->len,a
	// 	entp->buf
	// 	); */

	/*  //if (nread == -1) */
	/*   // break;
	// if (nread == 0)

	// break;
	//else{ */

	system_call(SYS_WRITE, STDOUT, entp->buf, strlen(entp->buf) );
	system_call(SYS_WRITE,STDOUT,"\n",1);

	entp = buf + entp->len;

	system_call(SYS_WRITE, STDOUT, entp->buf, strlen(entp->buf) );
	system_call(SYS_WRITE,STDOUT,"\n",1);

	/*   // }
	//} */

	system_call(SYS_CLOSE, fd);
	return 0;
}
