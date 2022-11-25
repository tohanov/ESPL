executable: addMulti

addMulti: addMulti.o
	gcc -m32 -Wall -g -o addMulti addMulti.o

addMulti.o: addMulti.s
	nasm -g -f elf -w+all -o addMulti.o addMulti.s

clean:
	rm -f *.o

r: clean executable
	
	./addMulti

# abc:
# 	nasm -g -f elf -w+all -o abc.o abc.s
# 	gcc -m32 -Wall -g -o abc abc.o
# 	./abc
	