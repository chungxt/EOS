#include <stdio.h>
#include <string.h>
#define fileName "text.txt"
#define bufSize 1024


int main(void){

	FILE *fp;
	fp = fopen(fileName, "r");
	char input[bufSize] = "";	
	char bufStr[bufSize] = "";
	char lastStr[bufSize] = "";
	
	char *endPos;
	char *containPos;
	
	
	if(fp == NULL){
		printf("File open error.\n");
	}
	else{
		printf("Please input search string:");
		scanf("%s", input);
	
		while((fgets(bufStr, sizeof(bufStr), fp)) != NULL){
			
			
			endPos = strchr(bufStr, '\n');
			
			if(endPos != NULL){ // end
				*endPos = '\0';
				
			}
			else { // not end (cut)
			}

			
			containPos = strstr(bufStr, input);

			while (containPos != NULL){

				char outStr[bufSize] = "";
				int spaceNum = 0;
				int lenFoward = (int)(containPos- bufStr);
				int lenBack = strlen(containPos);
				

				// forward
				char temp[bufSize] = "";
				int tempCount = 0;

				for(int i = lenFoward - 1; i>=0; i--){
					
					temp[tempCount] = bufStr[i];

					if(temp[tempCount] == ' '){
						spaceNum++;
					}
					tempCount++;
					if(spaceNum == 3){						
						break;
					}
					
				}
				
				
				for(int i = 0; i<tempCount; i++){ // 0 ~ (tempCount-1)
					
					outStr[i] = temp[(tempCount-1) - i];
					
				}

				

				// back
				spaceNum = 0;

   				for(int i = 0; i<lenBack; i++){

					outStr[i + tempCount] = containPos[i];
					
					if(outStr[i + tempCount] == ' '){
						spaceNum++;
					}
					
					if(spaceNum == 3){
						
						break;
					}
				}
				
				if(outStr[strlen(outStr)-1] == ' '){
					outStr[strlen(outStr)-1] = '\0'; // set last " " as '\0'
				}
				if(outStr[0] == ' '){
					for(int i = 0; i<strlen(outStr); i++){
						outStr[i] = outStr[i+1]; // skip first " "
					
					}
				}
				
				printf("%s\n", outStr);
				containPos = containPos+1;	// get containPos[1:]
				containPos = strstr(containPos, input);

			}

		}

		if(feof(fp)){
			printf("END\n");
		}
		else{

			printf("ERROR\n");
		}
		
		fclose(fp);

	}

	return 0;

}
