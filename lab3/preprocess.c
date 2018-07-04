#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define fileName "data.txt"
#define bufSize 20

int main () {

	
	
	// set 10000 number

	
	FILE *fp;
	fp = fopen(fileName, "w");
	
	
	srand(time(NULL));
	
	if(fp == NULL){
		printf("File open error.\n");
	}
	else{
		
		int i;
		for(i = 0; i < 15000; i++){
			int num = (rand() % 100) + 1;
			fprintf(fp, "%d\n", num);

		}
		
		fclose(fp);

	}


	return 0;
}
