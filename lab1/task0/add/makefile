all: main

main: main.o numbers.o add.o
	gcc -g -m32 -Wall -o main main.o numbers.o add.o

main.o: main.c
	gcc -g -m32 -Wall -c -o main.o main.c

numbers.o: numbers.c
	gcc -g -m32 -Wall -c -o numbers.o numbers.c

add.o: add.s
	nasm -g -f elf -w+all -o add.o add.s

clean:
	rm -rf ./*.o
