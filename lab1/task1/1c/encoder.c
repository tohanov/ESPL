#include <stdio.h>
#include <string.h>


typedef struct _Config {
	char inDebugMode;

	struct KeyInfo {
		signed char keySign;
		char* key;
	} keyInfo;

} Config;


void parseArguments(Config* config, int argc, char* argv[]) {
	/* static */ const char DEBUG_ARG[] = "-D";
	int i = 0;

	for (i = 1; i < argc; ++i) {
		if (argv[i][1] == 'e') {
			if (argv[i][0] == '+') {
				config->keyInfo.keySign = +1;
				config->keyInfo.key = argv[i] + 2;
			}
			else if (argv[i][0] == '-') {
				config->keyInfo.keySign = -1;
				config->keyInfo.key = argv[i] + 2;
			}
		}
		else if (strncmp(argv[i], DEBUG_ARG, sizeof(DEBUG_ARG)) == 0) {
			config->inDebugMode = 1;
		}
	}
}


char encode(Config* config, char inputChar) {
	static char diff = 'a' - 'A';
	static int keyIndex = 0;
	char encodedChar = inputChar;

	if (inputChar == '\n') {
		keyIndex = 0;
	}
	else if (config->keyInfo.keySign != 0) {
		// encoding with key
		encodedChar += config->keyInfo.keySign * (config->keyInfo.key[keyIndex] - '0');

		keyIndex = keyIndex + 1;
		if (config->keyInfo.key[keyIndex] == '\0') {
			keyIndex = 0;
		}
	}
	else if (inputChar <= 'z' && inputChar >= 'a') {
		encodedChar -= diff;
	}

	if (config->inDebugMode) {
		if (inputChar != '\n') {
			fprintf(stderr, "%.2X %.2X\n", inputChar, encodedChar);
		}
		else {
			fputc('\n', stderr);
		}
	}

	return encodedChar;
}


int main(int argc, char* argv[]) {
	char inputChar = '\0';
	char encodedChar = 0;
	int i = 0;

	Config config = {0, {0, NULL}};
	parseArguments(&config, argc, argv);

	if (config.inDebugMode) {
		for (i = 1; i < argc; ++i) {
			fprintf(stderr, "%s ", argv[i]);
		}

		fputc('\n', stderr);
	}

	while ( (inputChar = fgetc(stdin)) != EOF ) {
		encodedChar = encode(&config, inputChar);

		fputc(encodedChar, stdout);
	}

	return 0;
}
