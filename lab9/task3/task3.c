#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <fcntl.h>
#include <string.h>


typedef struct menu_item {
	char *desc;
	void (*f)(void);
} menu_item;


void examineELFFile(void);
void toggleDebugMod(void);
void printSectionNames(void);
void printSymbols(void);
void relocationTables(void);
void quit(void);
void executeMenu(menu_item *menu, int menuItemsCount);


int currentFd = -1;
void *map_start = MAP_FAILED;
char inDebugMode = 0;


Elf32_Ehdr *elfHeaderPointer;
Elf32_Shdr *sectionHeadersTable;

Elf32_Shdr *sectionHeadersStringTableSectionHeader;
const char * sectionHeadersStringTable;

Elf32_Sym *dynsymTable;
const char *dynstr;


int main() {
	menu_item menu[] = {
		{"Toggle Debug Mode", toggleDebugMod},
		{"Examine ELF File", examineELFFile},
		{"Print Section Names", printSectionNames},
		{"Print Symbols", printSymbols},
		{"Relocation Tables", relocationTables},
		{"Quit", quit},
		{NULL, NULL}
	};

	int menuItemsCount = sizeof(menu) / sizeof(menu_item) - 1;

	while (1) {
		executeMenu(menu, menuItemsCount);
	}

	return 0;
}


void executeMenu(menu_item *menu, int menuItemsCount) {
	char inputBuffer[256];

	printf("\nChoose action:\n");
	printf("-----------------\n");

	for (int i = 0; i < menuItemsCount; ++i) {
		printf("%d) %s\n", i, menu[i].desc);
	}

	int userChoice;

	printf("\nAction: ");
	fgets(inputBuffer, 256, stdin);
	sscanf(inputBuffer, "%d", &userChoice);

	putchar('\n');
	if (userChoice < 0 || userChoice >= menuItemsCount) {
		puts("Invalid choice.\n");
	}
	else {
		menu[userChoice].f();
	}
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


void printELFHeader() {
	printf("\n1) Bytes 1,2,3 in ascii: %.3s\n", (char *)(map_start + 1));
	
	// if (strncmp(map_start+1, "ELF", 3) != 0) {
	if (*((int*)map_start) != *((int*)ELFMAG)) {
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
	printf("8) Number of program-headers: %d\n", elfHeaderPointer->e_phnum);
	printf("9) Size of each program-header: %d\n", elfHeaderPointer->e_phentsize);
}


void initGlobals() {
	printf("[*] inside initGlobals()");
	
	elfHeaderPointer = (Elf32_Ehdr*)map_start;
	sectionHeadersTable = (Elf32_Shdr *)(map_start + elfHeaderPointer->e_shoff);

	sectionHeadersStringTableSectionHeader = &sectionHeadersTable[elfHeaderPointer->e_shstrndx];
	sectionHeadersStringTable = map_start + sectionHeadersStringTableSectionHeader->sh_offset;

	for (int i = 0; i < elfHeaderPointer->e_shnum; ++i) {
		if (sectionHeadersTable[i].sh_type == SHT_DYNSYM) {
			dynsymTable = (Elf32_Sym *)(map_start + sectionHeadersTable[i].sh_offset);
		}
		else if (sectionHeadersTable[i].sh_type == SHT_STRTAB
			&& strncmp(".dynstr", &sectionHeadersStringTable[sectionHeadersTable[i].sh_name], 8) == 0) {
				dynstr = map_start + sectionHeadersTable[i].sh_offset;
		}
	}
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

	initGlobals();
	printELFHeader();
}


void printSectionNames() {
	if (map_start == MAP_FAILED) {
		printf("No file is currently opened.\n");
		return;
	}
	
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



void printSymbols(void) {
	if (map_start == MAP_FAILED) {
		printf("No file is currently opened.\n");
		return;
	}

	for (int i = 0; i < elfHeaderPointer->e_shnum; ++i) {
		if ( ! (sectionHeadersTable[i].sh_type == SHT_SYMTAB || sectionHeadersTable[i].sh_type == SHT_DYNSYM)) {
			continue;
		}

		Elf32_Sym *symbolTable = (Elf32_Sym *)(map_start + sectionHeadersTable[i].sh_offset);
		const char *linkedStringsTable = (const char *)(map_start + sectionHeadersTable[sectionHeadersTable[i].sh_link].sh_offset);
		int entriesNumber = sectionHeadersTable[i].sh_size / sizeof(Elf32_Sym);

		if (inDebugMode) {
			printf("\ntable size: %d, number of symbols: %d\n", 
				sectionHeadersTable[i].sh_size, 
				entriesNumber
			);

			// [index] value section_index section_name symbol_name 
			printf("%5s\t%8s\t%8s\t%20s\t%20s\n",
				"Index",
				"Value",
				"Section Index",
				"Section Name",
				"Symbol Name"
			);
		}

		for (int j = 0; j < entriesNumber; ++j) {
			printf("[%3d]\t%08x\t%08d\t%20s\t%20s\n",
				// 	"Index",
				j,
				// value
				symbolTable[j].st_value,
				// section_index
				symbolTable[j].st_shndx,
				// section_name
				&sectionHeadersStringTable[sectionHeadersTable[i].sh_name],
				// // symbol_name
				&linkedStringsTable[symbolTable[j].st_name]
			);
		}
	}
}


void relocationTables(void) {
	if (map_start == MAP_FAILED) {
		printf("No file is currently opened.\n");
		return;
	}

	// Offset     Info    Type  Sym.Value  Sym. Name
	printf("%8s\t%8s\t%3s\t%8s\t%20s\n",
		"Offset",
		"Info",
		"Type",
		"Sym.Value",
		"Sym.Name"
	);

	for (int i = 0; i < elfHeaderPointer->e_shnum; ++i) {
		if (sectionHeadersTable[i].sh_type != SHT_REL) {
			continue;
		}
		
		Elf32_Rel *relocationTable = (Elf32_Rel *)(map_start + sectionHeadersTable[i].sh_offset);
		int entriesNumber = sectionHeadersTable[i].sh_size / sectionHeadersTable[i].sh_entsize;
		
		if (inDebugMode) {
			printf("\nRelocation section '%s' at offset %x contains %d entries:\n",
				&sectionHeadersStringTable[sectionHeadersTable[i].sh_name],
				sectionHeadersTable[i].sh_offset,
				entriesNumber
			);

			printf("%8s\t%8s\t%3s\t%8s\t%20s\n",
				"Offset",
				"Info",
				"Type",
				"Sym.Value",
				"Sym.Name"
			);
		}

		for (int j = 0; j < entriesNumber; ++j) {
			// Offset	Info	Type	Sym.Value	Sym. Name
			printf("%08x\t%08d\t%3d\t%08x\t%20s\n",
				relocationTable[j].r_offset,
				relocationTable[j].r_info,
				ELF32_R_TYPE(relocationTable[j].r_info),
				dynsymTable[ELF32_R_SYM(relocationTable[j].r_info)].st_value,
				&dynstr[dynsymTable[ELF32_R_SYM(relocationTable[j].r_info)].st_name]
			);
		}
	}
}