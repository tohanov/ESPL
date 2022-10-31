#include <stdio.h>
#include <string.h>


char gotDebugArgument(int argc, char* argv[]) {
	/* static */ const char DEBUG_ARG[] = "-D";
	int i = 0;

	for (i = 1; i < argc; ++i) {
		if (strncmp(argv[i], DEBUG_ARG, sizeof(DEBUG_ARG)) == 0) {
			return 1;
		}
	}

	return 0;
}


int main(int argc, char* argv[]) {
	char diff = 'a' - 'A';
	char inputChar = '\0';
	char encodedChar = 0;
	int i = 0;

	char inDebugMode = gotDebugArgument(argc, argv);

	if (inDebugMode) {
		for (i = 1; i < argc; ++i) {
			fprintf(stderr, "%s ", argv[i]);
		}

		fputs("\n", stderr);
	}

	while ( (inputChar = fgetc(stdin)) != EOF ) {
		encodedChar = inputChar;

		if (encodedChar <= 'z' && encodedChar >= 'a') {
			encodedChar -= diff;
		}

		if (inDebugMode) {
			if (inputChar != '\n') {
				fprintf(stderr, "%.2X %.2X\n", inputChar, encodedChar);
			}
			else {
				fputc(encodedChar, stderr);
			}
		}
		
		fputc(encodedChar, stdout);
	}

	// fputs("", stdout);
	// if (inDebugMode) {
	// 	fputs("", stderr);
	// }

	return 0;
}
