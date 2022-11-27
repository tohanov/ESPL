all:  task1b

task1b: task1b.o
	gcc -m32 -g -Wall -o task1b task1b.o

task1b.o: task1b.s 
	nasm -g -f elf -w+all -o task1b.o task1b.s 

.PHONY: clean  

clean:
	rm -f *.o task1b