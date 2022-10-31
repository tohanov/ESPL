#include <stdio.h>

int main(int argc, char* argv[]) {
	char diff = 'a' - 'A';
	char inputChar = '\0';

	while ( (inputChar = fgetc(stdin)) != EOF ) {
		if (inputChar <= 'z' && inputChar >= 'a') {
			inputChar -= diff;
		}

		fputc(inputChar, stdout);
	}

	return 0;
}
