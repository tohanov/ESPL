#include <stdlib.h>
#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _fun_desc {
	char *name;
	char (*fun)(char);
} func_desc; 

char censor(char c);
char* map(char *array, int array_length, char (*f) (char));

///////////////////////////////////////////////////////////////////////////////////////////////

char my_get(char c);
/* Ignores c, reads and returns a character from stdin using fgetc. */

char cprt(char c);
/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */

char encrypt(char c);
/* Gets a char c and returns its encrypted form by adding 3 to its value. If c is not between 0x20 and 0x7E it is returned unchanged */

char decrypt(char c);
/* Gets a char c and returns its decrypted form by reducing 3 to its value. If c is not between 0x20 and 0x7E it is returned unchanged */

char xprt(char c);
/* xprt prints the value of c in a hexadecimal representation followed by a new line, and returns c unchanged. */ 

char quit(char c);
/* Gets a char c, and if the char is 'q' , ends the program with exit code 0. Otherwise returns c. */ 

void menu();

///////////////////////////////////////////////////////////////////////////////////////////////

int main() {
	menu();

	return 0;
}


void menu() {
	char *carray = (char*)malloc(5 * sizeof(char));
	char *tmpCarray = NULL;
	char buf[256];
	int choice = 0;

	func_desc farray[] = {
		{"Get string", my_get},
		{"Print string", cprt},
		{"Print hex", xprt},
		{"Censor", censor},
		{"Encrypt", encrypt},
		{"Decrypt", decrypt},
		{"Quit", quit},
		{"Junk", &farray},
		{ NULL, NULL },
	};

	int bounds[] = {0, sizeof(farray) / sizeof(func_desc) - 2};
	// printf("%d\n", bounds[1]);

	while ( ! (choice < bounds[0] || choice > bounds[1])) {
		puts("Please choose a function:");

		for (int i = 0; farray[i].fun != NULL; ++i) {
			printf("%d)  %s\n", i, farray[i].name);
		}
		
		printf("Option: ");
		fgets(buf, 256, stdin);
		sscanf(buf, "%d", &choice);

		if (choice < bounds[0] || choice > bounds[1]) {
			puts("\nOutside of bounds");
		}
		else {
			puts("\nWithin bounds");

			tmpCarray = map(carray, 5, farray[choice].fun);
			free(carray);
			carray = tmpCarray;

			// printf("%0.5s", carray);

			puts("DONE.\n");
		}
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////

char checkC(char c) {
	return (c <= 0x7e && c >= 0x20);
}


char my_get(char c) {
	return (char)fgetc(stdin);
}


char cprt(char c) {
	char localC = c;

	if ( ! checkC(c)) {
		localC = '.';
	}

	printf("%c\n", localC);

	return c;
}


char encrypt(char c) {
	return checkC(c) ? c + 3 : c;
}


char decrypt(char c) {
	return checkC(c) ? c - 3 : c;
}


char xprt(char c) {
	
	if (checkC(c)) {
		printf("%.2X\n", c);
	}
	else {
		puts(".");
	}

	return c;
}


char quit(char c) {
	if (c == 'q') {
		exit(0);
	}

	return c;
}

///////////////////////////////////////////////////////////////////////////////////////////////

char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}


char* map(char *array, int array_length, char (*f) (char)) {
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  
  for (int i = 0; i < array_length; ++i) {
	mapped_array[i] = f(array[i]);
  }

  return mapped_array;
}
