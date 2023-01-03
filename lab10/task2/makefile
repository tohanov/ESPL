executable: loader


loader: loader.c
	gcc -Wall -m32 -g -c loader.c -o loader.o
	ld -o loader loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2 


clean:
	rm -rf loader.o loader


r:	clean loader
	clear
	# ./loader loadme
	# ./loader myshell
	./loader lab5task0b 1 2 3 4 5 asdfasd abcdefg