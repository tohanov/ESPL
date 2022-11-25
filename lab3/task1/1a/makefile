executable: virusDetector


virusDetector: virusDetector.c
	gcc -Wall -g -m32 -o virusDetector virusDetector.c


r: virusDetector

	./virusDetector


valg: virusDetector

	# echo ABCD > test

	clear
	valgrind -v --leak-check=full --show-reachable=yes \
		./virusDetector test

	# rm -f test