executable: main

main: main.o
	gcc -m32 -Wall -g main.o -o main

main.o: main.s
	nasm -g -f elf -w+all -o main.o main.s

clean:
	rm -f *.o

r: clean executable
	./main 1 2 3