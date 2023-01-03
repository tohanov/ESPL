#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>


int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
void testFunc(Elf32_Phdr *programHeader, int i) {
	printf("Program header number %d at address %x\n", i, programHeader->p_vaddr);
}
void printProgramHeaderInfo(Elf32_Phdr *programHeader, int i);


int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Filename wasn't provided.\n");
		exit(EXIT_FAILURE);
	}

	int descriptor = open(argv[1], O_RDONLY);

	if (descriptor == -1) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	off_t fileSize = lseek(descriptor, 0, SEEK_END);

	void *map_start = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, descriptor, 0);

	close(descriptor);

	printf("%8s\t%8s\t%8s\t%8s\t%8s\t%8s\t%8s\t%8s\n",
		"Type",
		"Offset",
		"VirtAddr",
		"PhysAddr",
		"FileSiz",
		"MemSiz",
		"Flg",
		"Align"
	);
	foreach_phdr(map_start, printProgramHeaderInfo, -1);

	return 0;
}


int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {

	Elf32_Ehdr *elfHeader = map_start;
	Elf32_Phdr *programHeaders = map_start + elfHeader->e_phoff;

	for (int i = 0; i < elfHeader->e_phnum; ++i) {
		func(&programHeaders[i], i);
	}

	return 0;
}


void printProgramHeaderInfo(Elf32_Phdr *programHeader, int i) {
	char readFlag = (programHeader->p_flags & PF_R) != 0;
	char writeFlag = (programHeader->p_flags & PF_W) != 0;
	char executeFlag = (programHeader->p_flags & PF_X) != 0;

	printf("%8d\t%08x\t%08x\t%08x\t%08x\t%08x\t%6c%c%c\t\t%08x\n",
		programHeader->p_type,
		programHeader->p_offset,
		programHeader->p_vaddr,
		programHeader->p_paddr,
		programHeader->p_filesz,
		programHeader->p_memsz,`
		// programHeader->p_flags,
		'R' * readFlag,
		'W' * writeFlag,
		'X' * executeFlag,
		programHeader->p_align
	);
}