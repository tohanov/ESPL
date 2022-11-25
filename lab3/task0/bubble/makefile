executable: bubblesort 

bubblesort: lab3_bubblesort.c
	gcc -Wall -g -m32 -o bubblesort lab3_bubblesort.c

r: bubblesort
	./bubblesort 3 4 2 1

valg: bubblesort
	clear
	valgrind -v --leak-check=full --show-reachable=yes \
		./bubblesort 3 4 2 1