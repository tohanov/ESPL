executable: printMulti

printMulti: printMulti.o
	gcc -m32 -Wall -g -o printMulti printMulti.o

printMulti.o: printMulti.s
	nasm -g -f elf -w+all -o printMulti.o printMulti.s

clean:
	rm -f *.o

r: clean executable
	
	./printMulti
	