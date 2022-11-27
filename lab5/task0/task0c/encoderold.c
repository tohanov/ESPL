#include <stdio.h>
#include <string.h>
// the firstZero and myStrlen  above I took from:  https://www.geeksforgeeks.org/write-your-own-strlen-for-a-long-string-padded-with-0s/?ref=rp
int firstZero(char str[], int low, int high)
{
	if (high >= low)
	{
		// Check if mid element is first '\0'
		int mid = low + (high - low)/2;
		if (( mid == 0 || str[mid-1] != '\0') && str[mid] == '\0')
			return mid;

		if (str[mid] != '\0') // If mid element is not '\0'
			return firstZero(str, (mid + 1), high);
		else // If mid element is '\0', but not first '\0'
			return firstZero(str, low, (mid -1));
	}
	return -1;
}

// This function mainly calls firstZero to find length.
int myStrlen(char str[])
{
    int n = sizeof(str)/sizeof(str[0]);
	// Find index of first zero in given stray
	int first = firstZero(str, 0, n-1);

	// If '\0' is not present at all, return n
	if (first == -1)
		return n;

	return first;
}

int main(int argc, char *argv[]) {
    int sign = 1;
    char* str;
    int size;
    int ddd = 5;
    int in_file = 1;
    int out_file =1;
    FILE *file1;
    FILE *file2;
    char *file_name;
    char ch;
    for (int i = 0; i < argc; i++) {

       if (argv[i][0] =='-' && argv[i][1]=='e') {
            sign = 2;
            str = argv[i];
            size= myStrlen(argv[i]) ;
            }
        if (argv[i][0] =='+' && argv[i][1]=='e' ) {
            sign = 3;
            str = argv[i];
            size= myStrlen(argv[i]) ;
            }
        if (argv[i][0] =='-'&& argv[i][1]=='D'){
            printf("%s \n", argv[i]);
            ddd = 0;
        }  
        if (argv[i][0] =='-'&& argv[i][1]=='i') {
            in_file = 2;
            file1 = fopen(argv[i]+2,"r");
            if (file1== NULL ){
               printf("can't open the file \n");
            }
        } 
         if (argv[i][0] =='-'&& argv[i][1]=='o') {
            out_file = 2;
            file2 = fopen(argv[i]+2,"w");
            if (file2== NULL ){
               printf("can't open the file \n");
            }
        }    
            
    }
    if (  sign > 1 ){
        int counter = 2;
        if (in_file == 1)
        {
            ch =  fgetc(stdin);
        }
        if (in_file == 2)
        {
           ch = fgetc(file1);
        }
        
        
        while (ch!=EOF) {
            int num = *(str + counter) - '0';
            if (sign == 2) {
                if (ch!= '\n'){
                    if (out_file == 2)
                    {
                       fputc(ch - num, file2);
                    }
                    else{
                        fputc(ch - num, stdout);
                    }
                    
                    if (counter==(size-1)){
                        counter = 2;
                    } else{
                        counter++;
                    }
                }
                else{
                    char *m = "\n";
                    
                    counter=2;
                     if (out_file == 2)
                    {
                       fputs("\n", file2);
                    }
                    else{
                       printf("%s",m);
                    }
                
                }  
                }
            if(sign==3){
                if (ch !='\n') {
                     if (out_file == 2)
                    {
                       fputc(ch + num, file2);
                    }
                    else{
                        fputc(ch + num, stdout);
                    }
                    if (counter==(size-1))
                        counter = 2;
                    else{
                        counter++;
                    }
                }else{
                    char *m = "\n";
                    counter=2;
                     if (out_file == 2)
                    {
                       fputs(m, file2);
                    }
                     else{
                       printf("%s",m);
                    }
                    counter = 2;
                }
                   
                      }
          if (in_file == 1)
        {
            ch = fgetc(stdin);
        }
        else{
            ch = fgetc(file1);
        }
                    }     
                }
    if (sign < 2){
        char c;
        if (in_file == 1)
        {
            c = fgetc(stdin);
        }
        else{
            c = fgetc(file1);
        }
        while (c != EOF) {
        if (c >= 'a' && c <= 'z') {
            if (ddd == 0) {//if -D is on
                fprintf(stderr, "%x %x\n", c, c - 32);
            }
             if (out_file == 2)
                    {
                       fputc(c-32, file2);
                    }
                    else{
                        fputc(c-32, stdout);
                    }
        }

        else{// upper already
            if (ddd == 0) {//if -D is on
                if (c!= '\n')
                    fprintf(stderr, "%x %x\n", c, c );
            }
             if (out_file == 2)
                    {
                       fputc(c, file2);
                    }
                    else{
                        fputc(c, stdout);
                    }
        }
        if (in_file == 1)
        {
            c = fgetc(stdin);
        }
        else{
            c = fgetc(file1);
        }
    }


    }
    return 0;
    fclose(file1);
    fclose(file2);

            }

    
    

