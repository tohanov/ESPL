executable: myshell

myshell: myshell.c LineParser.c
	gcc -m32 -g -Wall -o myshell myshell.c LineParser.c

clean:
	rm -f *.o

r: clean executable
	./myshell

