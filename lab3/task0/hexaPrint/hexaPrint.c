
// fread(3) to read data from the file into memory.
// A helper function, PrintHex(buffer, length), that prints length bytes from memory location buffer, in hexadecimal format.

#include <stdio.h>
#include <stdlib.h>


void PrintHex(void *buffer, size_t length);


int main(int argc, char **argv) {
	FILE *fileDescriptor = NULL;
	char *fileContents = NULL;
	size_t fileSize = 0;

	if (argc < 2) {
		fputs("File name not provided in command line arguments. Exiting.\n", stderr);
		return EXIT_FAILURE;
		// exit(EXIT_FAILURE);
	}

	fileDescriptor = fopen(argv[1], "rb");

	fseek(fileDescriptor, 0, SEEK_END);
	fileSize = ftell(fileDescriptor);

	fseek(fileDescriptor, 0, SEEK_SET);
	fileSize *= sizeof(char);

	fileContents = malloc(fileSize);

	if (fileContents == NULL) {
		fputs("Memory failed to allocate. Exiting.\n", stderr);
		return EXIT_FAILURE;
	}

	/* int a =  */
	fread(fileContents, 1, fileSize, fileDescriptor);
	// printf("a = %d\n", a);
	fclose(fileDescriptor);

	PrintHex(fileContents, fileSize);
	fputc('\n', stdout);

	free(fileContents);

	return 0;
}


void PrintHex(void *buffer, size_t length) {
	int i = 0;
	unsigned char *ucBuf = buffer;

	for (i = 0; i < length; ++i) {
		printf("%.2X ", ucBuf[i]);
	}
}