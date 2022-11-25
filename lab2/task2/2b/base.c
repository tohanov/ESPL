#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 

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
	printf("%.2X\n", c);

	return c;
}


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
 
int main(int argc, char **argv) {
	int base_len = 5;
	char arr1[base_len];
	
	char* arr2 = map(arr1, base_len, my_get);
	char* arr3 = map(arr2, base_len, cprt);
	char* arr4 = map(arr3, base_len, xprt);
	char* arr5 = map(arr4, base_len, encrypt);
	char* arr6 = map(arr5, base_len, decrypt);

	free(arr2);
	free(arr3);
	free(arr4);
	free(arr5);
	free(arr6); 
}

