executable: virusDetector


virusDetector: virusDetector.c
	gcc -Wall -g -m32 -o virusDetector virusDetector.c


r: virusDetector

	clear
	./virusDetector infected


valg: virusDetector

	# echo ABCD > test

	clear
	valgrind -v --leak-check=full --show-reachable=yes \
		./virusDetector infected

	# rm -f test