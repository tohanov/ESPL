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

	foreach_phdr(map_start, testFunc, -1);

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