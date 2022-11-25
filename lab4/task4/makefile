executable: lfsr

lfsr: lfsr.o
	gcc -m32 -Wall -g -o lfsr lfsr.o

lfsr.o: lfsr.s
	nasm -g -f elf -w+all -o lfsr.o lfsr.s

clean:
	rm -f *.o

r: clean executable
	
	./lfsr

# abc:
# 	nasm -g -f elf -w+all -o abc.o abc.s
# 	gcc -m32 -Wall -g -o abc abc.o
# 	./abc
	