#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <errno.h>


int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
// void testFunc(Elf32_Phdr *programHeader, int i) {
// 	printf("Program header number %d at address %x\n", i, programHeader->p_vaddr);
// }
void printProgramHeaderInfo(Elf32_Phdr *programHeader, int i);
void printProgramHeadersTitles(void);
void load_phdr(Elf32_Phdr *phdr, int fd);


extern int startup(int argc, char **argv, void (*start)());


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

	void *map_start = mmap(NULL, fileSize, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE, descriptor, 0);
	
	if (map_start == MAP_FAILED) {
		perror("[!] mmap()");
		return;
	}

	Elf32_Ehdr *elfHeader = map_start;

	printf("map_start = %p\n", (void *)map_start);

	foreach_phdr(map_start, load_phdr, descriptor);

	puts("========================================================================================================================");
	printf("[*] calling startup() with parameters: argc=%d, start()=e_entry=%p\n\n", argc, (void *)elfHeader->e_entry);
	startup(argc - 1, argv + 1, (void *)elfHeader->e_entry);

	munmap(map_start, fileSize);
	close(descriptor);
	return 0;
}


int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {
	Elf32_Ehdr *elfHeader = map_start;
	Elf32_Phdr *programHeaders = map_start + elfHeader->e_phoff;

	for (int i = 0; i < elfHeader->e_phnum; ++i) {
		func(&programHeaders[i], arg);
	}

	return 0;
}


void printProgramHeaderInfo(Elf32_Phdr *programHeader, int arg) {
	char readFlag = (programHeader->p_flags & PF_R) != 0;
	char writeFlag = (programHeader->p_flags & PF_W) != 0;
	char executeFlag = (programHeader->p_flags & PF_X) != 0;

	printf("%8d\t%08x\t%08x\t%08x\t%08x\t%08x\t%2c%1c%1c\t%08x\n",
		programHeader->p_type,
		programHeader->p_offset,
		programHeader->p_vaddr,
		programHeader->p_paddr,
		programHeader->p_filesz,
		programHeader->p_memsz,
		// programHeader->p_flags,
		'R' * readFlag,
		'W' * writeFlag,
		'X' * executeFlag,
		programHeader->p_align
	);
}


void printProgramHeadersTitles(void) {
	printf("%8s\t%8s\t%8s\t%8s\t%8s\t%8s\t%4s\t%8s\n",
		"Type",
		"Offset",
		"VirtAddr",
		"PhysAddr",
		"FileSiz",
		"MemSiz",
		"Flg",
		"Align"
	);
}


void load_phdr(Elf32_Phdr *phdr, int fd) {
	puts("========================================================================================================================");
	printProgramHeadersTitles();
	printProgramHeaderInfo(phdr, fd);
	putchar('\n');
	
	if (phdr->p_type != PT_LOAD) {
		printf("[*] Program header is not of type PT_LOAD; skipping\n");
		return;
	}

	Elf32_Addr vaddr = phdr->p_vaddr & 0xfffff000;
	Elf32_Off offset = phdr->p_offset & 0xfffff000;
	Elf32_Addr padding = phdr->p_vaddr & 0xfff;

	char readFlag = (phdr->p_flags & PF_R) != 0;
	char writeFlag = (phdr->p_flags & PF_W) != 0;
	char executeFlag = (phdr->p_flags & PF_X) != 0;

	printf("[*] Attempting to load with flags: r=%d,w=%d,e=%d; bitwise-or result: %d\n",
		PROT_READ * readFlag,
		PROT_WRITE * writeFlag,
		PROT_EXEC * executeFlag,
		PROT_READ * readFlag | PROT_WRITE * writeFlag | PROT_EXEC * executeFlag
	);

	void *map_start = mmap(
		(void *)vaddr,
		phdr->p_memsz + padding,
		PROT_READ * readFlag | PROT_WRITE * writeFlag | PROT_EXEC * executeFlag,
		MAP_FIXED | MAP_PRIVATE,
		fd,
		offset
	);

	if (map_start == MAP_FAILED) {
		perror("[!] mmap()");
		printf("[*] errno == EPERM: %s\n", errno == EPERM ? "True" : "False");
		return;
	}

	printf("[*] map succeeded; map_start == vaddr: %s\n", map_start == (void *)vaddr ? "True" : "False");
}