
// virus* readVirus(FILE*): this function receives a file pointer and returns a virus* that represents the next virus in the file. To read from a file, use fread(). See man fread(3) for assistance.
// void printVirus(virus* virus, FILE* output): this function receives a virus and a pointer to an output file. The function prints the virus to the given output. It prints the virus name (in ASCII), the virus signature length (in decimal), and the virus signature (in hexadecimal representation).



// Open the signatures file, use readVirus in order to read the viruses one-by-one, and use printVirus in order to print the virus (to a file or to the standard output, up to your choice).
// Test your implementation by comparing your output with the file.


#include <stdio.h>
#include <stdlib.h>

#define bufsize 256


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


typedef struct {
	char *description;
	void (*function)(link **);
} menuFunction;


// ============== 1a
virus* readVirus(FILE *file);
void PrintHex(void *buffer, size_t length, FILE *output);
void printVirus(virus *virus, FILE *output);

void freeVirus(virus *virus);
// ============== 1a


// ============== 1b
void list_print(link *virus_list, FILE*);
/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
link* list_append(link* virus_list, virus* data);
/* Add a new link with the given data to the list (either at the end or the beginning, depending on what your TA tells you), and return a pointer to the list (i.e., the first link in the list). If the list is null - create a new entry and return a pointer to the entry. */
void list_free(link *virus_list);
/* Free the memory allocated by the list. */

void loadSignatures(link **signaturesList);
void printSignatures(link **signaturesList);
void quit(link **signaturesList);
void notImplemented(link **signaturesList);
// ============== 1b


int main(int argc, char **argv) {
	// FILE *signaturesFile = fopen("../signatures-L", "rb");
	// virus *iterVirus = NULL;

	// fseek(signaturesFile, 4, SEEK_SET);

	// while ((iterVirus = readVirus(signaturesFile)) != NULL) {
	// 	printVirus(iterVirus, stdout);
	// 	fputc('\n', stdout);
	// 	freeVirus(iterVirus);
	// }

	// fclose(signaturesFile);

	// return 0;

	char buffer[bufsize];
	link **signaturesList = malloc(sizeof(link *));
	int choice = 0;

	*signaturesList = NULL; // need to initialize, since performing checks on value later

	menuFunction menuItems[] = {
		{"Load signatures", loadSignatures},
		{"Print signatures", printSignatures},
		{"Detect viruses", notImplemented},
		{"Fix file", notImplemented},
		{"Quit", quit},
		{ NULL, NULL },
	};

	int bounds[] = {
		1 + 0, 
		1 + sizeof(menuItems) / sizeof(menuFunction) - 2
	};

	while ( 1 ) {
		puts("Please choose a function:");

		for (int i = 0; menuItems[i].function != NULL; ++i) {
			printf("%d)  %s\n", i+1, menuItems[i].description);
		}
		
		printf("Option: ");
		fgets(buffer, 256, stdin);
		sscanf(buffer, "%d", &choice);

		if (choice < bounds[0] || choice > bounds[1]) {
			puts("\nOutside of bounds.");
		}
		else {
			// puts("\nWithin bounds");
			menuItems[choice - 1].function(signaturesList);

			puts("DONE.\n");
		}
	}

	return 0;
}


// ============== 1a

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

	fputs("\n", output);
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

// ============== 1a


// ============== 1b

void list_free(link *virus_list) {
	link *iterator = virus_list;
	link *temp = NULL;

	while (iterator != NULL) {
		temp = iterator->nextVirus;

		freeVirus(iterator->vir);
		free(iterator);

		iterator = temp;
	}
}


void list_print(link *virus_list, FILE *output) {
	link *iterator = virus_list;

	while (iterator != NULL) {
		printVirus(iterator->vir, output);
		fputs("\n", output);
		iterator = iterator->nextVirus;
	}
}


link* list_append(link* virus_list, virus* data) {
	link *newHead = malloc(sizeof(link));

	newHead->vir = data;
	newHead->nextVirus = virus_list;
	
	return newHead;
}

// -------------- menu
void quit(link **signaturesList) {
	list_free(*signaturesList);
	free(signaturesList);

	exit(EXIT_SUCCESS);
}


void notImplemented(link **signaturesList) {
	fputs("Not implemented\n", stderr);
}


FILE *getFileDescriptor() {
	#define bufsize 256

	char buffer[bufsize];
	char filePath[bufsize];
	
	fputs("Path to file: ", stdout);
	fgets(buffer, bufsize, stdin);
	sscanf(buffer, "%s", filePath);

	FILE *signaturesFile = fopen(filePath, "rb");

	if (signaturesFile == NULL) {
		// fputs("File could not be opened.\n", stderr);
		fprintf(stderr, "File %s could not be opened.\n", filePath);
	}

	return signaturesFile;
}


void loadSignatures(link **signaturesList) {
	FILE *signaturesFile = getFileDescriptor();
	virus *iterVirus = NULL;

	if (signaturesFile != NULL) {
		fseek(signaturesFile, 4, SEEK_SET);

		while ((iterVirus = readVirus(signaturesFile)) != NULL) {
			*signaturesList = list_append(*signaturesList, iterVirus);
		}

		fclose(signaturesFile);
	}
}


void printSignatures(link **signaturesList) {
	fputc('\n', stdout);

	list_print(*signaturesList, stdout);
}
// -------------- menu

// ============== 1b