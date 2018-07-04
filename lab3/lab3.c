#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.c"

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

#define readDataName "data.txt"
#define readRuleName "rule.txt"
#define bufSize 20

int main () {

	LinkedList *list = new_LinkedList();
	int count = 0;
	
	FILE *fp;
	int ruleNum = 0;
	int *ruleValue;
	

	int fd_led;     // file descriptor for /dev/lcd 
        unsigned short data_led;
	_7seg_info_t    data_7seg;
	lcd_write_info_t display; // saving LCD data 
	int ret; // keypad
	unsigned short  key;

        // open device /dev/lcd
        if((fd_led = open("/dev/lcd", O_RDWR)) < 0)
        {
            printf("Open /dev/lcd faild. \n");
            exit(-1);
        }	
	
	ioctl(fd_led, KEY_IOCTL_CLEAR, key);
	
	while(1)
    	{
        	/*
		ret = ioctl(fd_led, KEY_IOCTL_CHECK_EMTPY, &key);
        	if(ret < 0)
		{
		    sleep(1);
		    continue;
		}
		*/
		ret = ioctl(fd_led, KEY_IOCTL_GET_CHAR, &key);
		// printf("%d\n", ret);		
		if(ret == 0)
		{
		    break; // start program
		}

	}

	
	printf("start program. \n");


	fp = fopen(readRuleName, "r");

	if(fp == NULL){
		printf("File open error.\n");
	}
	else{

		while(!feof(fp)){
			int num;
			fscanf(fp, "%d\n", &num);
			ruleNum++;
			
		}
		

	}
	fclose(fp);

	fp = fopen(readRuleName, "r");

	if(fp == NULL){
		printf("File open error.\n");
	}
	else{
		
		ruleValue = malloc(ruleNum * sizeof(int));
		ruleNum = 0;
		while(!feof(fp)){
			fscanf(fp, "%d\n", &ruleValue[ruleNum]);
			ruleNum++;
		}
		

	}
	fclose(fp);


	fp = fopen(readDataName, "r");

	if(fp == NULL){
		printf("File open error.\n");
	}
	else{

		ioctl(fd_led, LCD_IOCTL_CLEAR, NULL);
		while(!feof(fp)){
						
			int taskPriority, taskRank;
			fscanf(fp, "%d\n", &taskPriority);
			printf("%d\n", taskPriority);

			char bufStr[bufSize];
			
			sprintf(bufStr, "%d", taskPriority);

			int i;
			for(i = 0; i < ruleNum; i++){

				
				if(ruleValue[i] != 0 && taskPriority % ruleValue[i] == 0){
					taskRank = i;
					
					if(i == 0){	// rule 1
						int j;
						for(j = 0; j < 2; j++){
							// turn on D9 
							data_led = LED_D9_INDEX;
							ioctl(fd_led, LED_IOCTL_BIT_SET, &data_led);
							printf("Turn on D9 \n");
							sleep(3);

							// turn off D9
							data_led = LED_D9_INDEX;
							ioctl(fd_led, LED_IOCTL_BIT_CLEAR, &data_led);
							printf("Turn off D9 \n");
							sleep(3);
						}
						ioctl(fd_led, _7SEG_IOCTL_ON, NULL);


						data_7seg.Mode = _7SEG_MODE_PATTERN;
					    	data_7seg.Which = _7SEG_ALL;
					    	data_7seg.Value = 0x08500806;    // change to _R_1
					    	ioctl(fd_led, _7SEG_IOCTL_SET, &data_7seg);
					    	sleep(3);

						data_7seg.Mode = _7SEG_MODE_HEX_VALUE;
					    	data_7seg.Which = _7SEG_ALL;
					    	data_7seg.Value = 0x0000;    // change to _R_1
					    	ioctl(fd_led, _7SEG_IOCTL_SET, &data_7seg);

					}
					break;
				    	
				}
				if(ruleValue[i] == 0 && i == ruleNum - 1){
					taskRank = i;
				}
				

			}


			

			if(count % 4 != 0 || count == 0){
				enqueue_Node(list, taskPriority, taskRank, bufStr);
				count++;
			}
			else{
				char str[200];
				dequeue_Node(list, str);

				count--;
				/*
				// clear LCD
			    	ioctl(fd_led, LCD_IOCTL_CLEAR, NULL);
			   	// save output string
			   	display.Count = sprintf((char *)display.Msg, str);
			    	// print
			    	ioctl(fd_led, LCD_IOCTL_WRITE, &display);
				*/
				enqueue_Node(list, taskPriority, taskRank, bufStr);
				count++;			
			}

			int num[4];
			char taskLcd[200];

			print_LinkedList (list, num);
			// clear LCD
			ioctl(fd_led, LCD_IOCTL_CLEAR, NULL);

			for(i = 0; i < count; i++){
				sprintf(taskLcd, "%d", num[i]);
							
				// save output string
				display.Count = sprintf((char *)display.Msg, taskLcd);
				// print
				ioctl(fd_led, LCD_IOCTL_WRITE, &display);
			}
			
			printf("\n");
			
		}
		
		delete_LinkedList (list);
		
		

	}
	fclose(fp);
	// end of file


	close(fd_led);

	return 0;
}
