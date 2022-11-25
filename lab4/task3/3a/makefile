executable: addMulti

addMulti: addMulti.o
	gcc -m32 -Wall -g -o addMulti addMulti.o

addMulti.o: addMulti.s
	nasm -g -f elf -w+all -o addMulti.o addMulti.s

clean:
	rm -f *.o

r: clean executable
	
	./addMulti
	