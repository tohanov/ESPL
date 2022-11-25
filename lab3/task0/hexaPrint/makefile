executable: hexaPrint


hexaPrint: hexaPrint.c
	gcc -Wall -g -m32 -o hexaPrint hexaPrint.c


r: hexaPrint

	# echo ABCD > test
	# cat test
	./hexaPrint exampleFile
	# rm -f test


valg: hexaPrint

	# echo ABCD > test

	clear
	valgrind -v --leak-check=full --show-reachable=yes \
		./hexaPrint exampleFile

	# rm -f test