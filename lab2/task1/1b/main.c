#include <stdio.h>

int main() {
	int iarray[3];
	float farray[3];
	double darray[3];
	char carray[3];

	// iarray, iarray+1, farray, farray+1, darray, darray+1, carray and carray+1 
	printf("iarray \t\t%p\n", iarray);
	printf("iarray+1 \t%p\n", iarray+1);
	printf("farray \t\t%p\n", farray);
	printf("farray+1 \t%p\n", farray+1);
	printf("darray \t\t%p\n", darray);
	printf("darray+1 \t%p\n", darray+1);
	printf("carray \t\t%p\n", carray);
	printf("carray+1 \t%p\n", carray+1);

	return 0;
}