#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{
  //char *my_string = itoa(argc);
  //system_call(SYS_WRITE,STDOUT, my_string,strlen(my_string));
  //system_call(SYS_WRITE,STDOUT," arguments \n", 12);
  int i = 0;
  while (i < argc)
  {
    //system_call(SYS_WRITE,STDOUT,"argv[", 5);
	   // my_string = itoa(i);
      //int length = strlen(my_string);
      //system_call(SYS_WRITE,STDOUT,my_string,length);
      //system_call(SYS_WRITE,STDOUT,"] = ",4);
      system_call(SYS_WRITE,STDOUT,argv[i],strlen(argv[i]));
      system_call(SYS_WRITE,STDOUT,"\n",1);
      i++;
  }

  return 0;
}
