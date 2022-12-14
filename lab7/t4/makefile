executable: myshell


myshell: myshell.c LineParser.c looper.c
	gcc -m32 -g -Wall -o myshell myshell.c LineParser.c
	gcc -m32 -g -Wall -o looper looper.c


clean:
	rm -f *.o
	rm -f myshell


r: clean executable
	clear
	./myshell


d: clean executable
	clear
	./myshell -d


v: clean executable
	clear
	valgrind -v --leak-check=full \
		./myshell -d