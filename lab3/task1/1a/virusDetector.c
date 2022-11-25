
// virus* readVirus(FILE*): this function receives a file pointer and returns a virus* that represents the next virus in the file. To read from a file, use fread(). See man fread(3) for assistance.
// void printVirus(virus* virus, FILE* output): this function receives a virus and a pointer to an output file. The function prints the virus to the given output. It prints the virus name (in ASCII), the virus signature length (in decimal), and the virus signature (in hexadecimal representation).



// Open the signatures file, use readVirus in order to read the viruses one-by-one, and use printVirus in order to print the virus (to a file or to the standard output, up to your choice).
// Test your implementation by comparing your output with the file.


#include <stdio.h>
#include <stdlib.h>


typedef struct virus {
	unsigned short SigSize;
	char virusName[16];
	unsigned char* sig;
} virus;


typedef struct link link;
struct link {
	link *nextVirus;
	virus *vir;
};


virus* readVirus(FILE *file);
void PrintHex(void *buffer, size_t length, FILE *output);
void printVirus(virus *virus, FILE *output);

void freeVirus(virus *virus);


int main(int argc, char **argv) {
	FILE *signaturesFile = fopen("../signatures-L", "rb");
	virus *iterVirus = NULL;

	fseek(signaturesFile, 4, SEEK_SET);

	while ((iterVirus = readVirus(signaturesFile)) != NULL) {
		printVirus(iterVirus, stdout);
		
		freeVirus(iterVirus);
	}

	fclose(signaturesFile);

	return 0;
}


void freeVirus(virus *virus) {
	free(virus->sig);
	free(virus);
}

void PrintHex(void *buffer, size_t length, FILE *output) {
	int i = 0;
	unsigned char *ucBuf = buffer;

	for (i = 0; i < length; ++i) {
		fprintf(output, "%.2X ", ucBuf[i]);
	}
}


void printVirus(virus *virus, FILE *output) {
	char *format =
		"Virus name: %s\n"
		"Virus size: %d\n"
		"signature:\n";

	fprintf(output, format, virus->virusName, virus->SigSize);

	PrintHex(virus->sig, virus->SigSize, output);

	fputs("\n\n", output);
}


virus* readVirus(FILE *file) {
	virus *nextVirus = malloc(sizeof(virus));
	int chunksRead = fread(nextVirus, sizeof(nextVirus->SigSize) + sizeof(nextVirus->virusName), 1, file);

	if (chunksRead == 0) {
		free(nextVirus);
		return NULL;
	}

	nextVirus->sig = malloc(nextVirus->SigSize);
	fread(nextVirus->sig, nextVirus->SigSize, 1, file);

	return nextVirus;
}