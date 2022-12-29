#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <fcntl.h>


typedef struct menu_item {
	char *desc;
	void (*f)(void);
} menu_item;


void examineELFFile(void);
void toggleDebugMod(void);
void printSectionNames(void);
void quit(void);


int currentFd = -1;
void *map_start = MAP_FAILED;
char inDebugMode = 0;


int main() {
	char inputBuffer[256];
	
	menu_item menu[] = {
		{"Toggle Debug Mode", toggleDebugMod},
		{"Examine ELF File", examineELFFile},
		{"Print Section Names", printSectionNames},
		// {"Examine ELF file", examineELFFile},
		{"Quit", quit},
		{NULL, NULL}
	};

	int menuItemsCount = sizeof(menu) / sizeof(menu_item) - 1;

	while (1) {
		printf("\nChoose action:\n");
		printf("-----------------\n");

		for (int i = 0; i < menuItemsCount; ++i) {
			printf("%d) %s\n", i, menu[i].desc);
		}

		int userChoice;

		printf("\nAction: ");
		fgets(inputBuffer, 256, stdin);
		sscanf(inputBuffer, "%d", &userChoice);

		// printf("read choice: %d\n", userChoice);

		putchar('\n');
		if (userChoice < 0 || userChoice >= menuItemsCount) {
			puts("Invalid choice.\n");
		}
		else {
			menu[userChoice].f();
		}
	}

	return 0;
}


void closeAndUnmap() {
	if (map_start != NULL) { 
		munmap(map_start, 52);
		map_start = MAP_FAILED;
	}

	if (currentFd != -1) {
		close(currentFd);
		currentFd = -1;
	}
}


void quit() {
	closeAndUnmap();
	exit(EXIT_SUCCESS);
}


void toggleDebugMod() {
	inDebugMode = !inDebugMode;
	printf(
		"Now %s debug mode\n", 
		inDebugMode ? "in" : "out of"
	);
}


void examineELFFile() {
	char rawInputBuffer[256];
	char fileName[256];

	closeAndUnmap();

	printf("File path: ");
	fgets(rawInputBuffer, 256, stdin);
	sscanf(rawInputBuffer, "%s", fileName);

	currentFd = open(fileName, O_RDWR);

	if (currentFd == -1) {
		perror("Error in open()");
		return;
	}

	off_t fileSize = lseek(currentFd, 0, SEEK_END);
	map_start = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, currentFd, 0);

	if (map_start == MAP_FAILED) {
		closeAndUnmap();
		perror("Error in mmap()");
		return;
	}

	Elf32_Ehdr *elfHeaderPointer = (Elf32_Ehdr*)map_start;

	printf("\n1) Bytes 1,2,3 in ascii: %.3s\n", (char *)(map_start + 1));
	
	if (*((int*)map_start) != *((int*)ELFMAG)) {
	// if (strncmp(map_start+1, "ELF", 3) != 0) {
		printf("Not an ELF file, stopping examination.\n");
		return;
	}
	printf("2) Data encoding: %s-bit, %s-endian\n", 
		elfHeaderPointer->e_ident[EI_CLASS] == ELFCLASS32 ? "32" : "64", 
		elfHeaderPointer->e_ident[EI_DATA] == ELFDATA2LSB ? "Little" : "Big"
	);

	printf("3) Entry point: %08X\n", elfHeaderPointer->e_entry);

	printf("4) Section-header table's offset: %08X\n", elfHeaderPointer->e_shoff);
	printf("5) Number of section-headers: %d\n", elfHeaderPointer->e_shnum);
	printf("6) Size of each section-header: %d\n", elfHeaderPointer->e_shentsize);

	printf("7) Program-header table's offset: %08X\n", elfHeaderPointer->e_phoff);
	printf("8) Number of section-headers: %d\n", elfHeaderPointer->e_phnum);
	printf("9) Size of each program-header: %d\n", elfHeaderPointer->e_phentsize);
}


void printSectionNames() {
	if (map_start == MAP_FAILED) {
		printf("No file is currently opened.\n");
	}

	Elf32_Ehdr *elfHeaderPointer = (Elf32_Ehdr *)map_start;
	Elf32_Shdr *sectionHeadersTable = (Elf32_Shdr *)(map_start + elfHeaderPointer->e_shoff);
	Elf32_Shdr *sectionHeadersStringTableSection = &sectionHeadersTable[elfHeaderPointer->e_shstrndx];
	const char * sectionHeadersStringTable = map_start + sectionHeadersStringTableSection->sh_offset;
	
	printf("%s\t%20s\t%8s\t%8s\t%8s\t%s\n",
		"Index",
		"Name",
		"Addr",
		"Offset",
		"Size",
		"Type"
	);

	for (int i = 0; i < elfHeaderPointer->e_shnum; ++i) {
		printf("[%2d]\t%20s\t%08X\t%08X\t%08X\t%d\n",
			i, 
			&sectionHeadersStringTable[sectionHeadersTable[i].sh_name],
			sectionHeadersTable[i].sh_addr,
			sectionHeadersTable[i].sh_offset,
			sectionHeadersTable[i].sh_size,
			sectionHeadersTable[i].sh_type
		);
	}

	// Elf32_Phdr *programHeaders = (Elf32_Phdr *)(map_start + elfHeaderPointer->e_phoff);

	// for (int i = 0; i < elfHeaderPointer->e_shnum; ++i) {
	// 	printf("\t[%d] %x\n", i, programHeaders[i].p);
	// }
}