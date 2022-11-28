all: task2

task2: main.o util.o start.o
	ld -m elf_i386 start.o main.o util.o -o task2 

main.o: main.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o 

util.o: util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o 	

start.o: start.s
	nasm -f elf start.s -o start.o

####################################################################

clean: clean_o clean_exec

clean_o: 
	rm -f *.o

clean_exec: 
	rm -f task2

####################################################################

r: clean task2 clean_o
	# clear
	# ./task2
	clear
	./task2 -aabc
