executable: mypipeline

mypipeline: mypipeline.c 
	gcc -m32 -g -Wall -o mypipeline mypipeline.c

clean:
	rm -f *.o
	rm -f mypipeline

r: clean executable
	clear
	./mypipeline -d

d: clean executable
	clear
	./mypipeline -d

v: clean executable
	clear
	valgrind -v --leak-check=full \
		./mypipeline -d