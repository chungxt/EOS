/* 
 * ubers.c
 */
 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

#include "LinkedList.c"

#define UBERS_NUM 5
#define MAP_SIZE 9
#define bufSize 200

LinkedList *requestList = NULL;
LinkedList *uberList = NULL;
ListNode *nowRequest = NULL;
int getRequest = 0;
int serveRequest = 0;
int fd; 


void *threadUberQueue(void *threadid)
{

	// check list number != 0

	long tid;
	tid = (long)threadid;
	printf("#%ld start \n", tid);
	int i = 0;
	int x, y;
	char str[bufSize];

	int requestNum = 0;
	int taskPriority = 0;
	int taskRank = 0;
	char bufStr[] = "UBER";

	srand(time(NULL));
	
	
	while(i < MAP_SIZE){
		x = rand() % MAP_SIZE +1;
		y = rand() % MAP_SIZE +1;
		
		enqueue_Node(uberList, taskPriority, taskRank, bufStr, x, y, 0, 0);
		i++;
		printf("(%d, %d) ", x, y);
		
	}
	printf("\n");
	
	print_LinkedList(uberList);

	while(1){
		ListNode *now;
		now = requestList->head;
		if(now != NULL){
			printf("#%ld get request \n", tid);
			nowRequest = now;
			getRequest = 1;

			// 1. confirm driving thread has been create
			// 2. wait for correct nowX, nowY 
			// (especially multiple requests)
			
			while(1){
				if(getRequest == 0 && serveRequest == 1){	
					serveRequest = 0;
					break;
				}
			}	
			dequeue_Node(requestList, str);
			print_LinkedList(requestList);
			// break;
			
			
		}
	}
	printf("#%ld end \n", tid);
	pthread_exit(NULL);
	
}



void *threadCustomerRequest(void *threadid)
{
	// get num from keypad
	// check if button is clicked for 4 times (use loop), then list enqueue
	long tid;
	tid = (long)threadid;
	printf("#%ld start \n", tid);
		
	int requestNum = 0;
	int taskPriority = 0;
	int taskRank = 0;
	char bufStr[] = "REQUEST";
	int startX, startY, endX, endY;
	int i = 0;
	int ret;

	while(1){
		
		unsigned short  key;
		ioctl(fd, KEY_IOCTL_CLEAR, key);
	
		for(i = 0; i < 4; i++)
	    	{
			while(1)
			{	
				ret = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);
				if(ret < 0){
					continue;
				}

				ret = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
				
				int num = 0;				
				switch(key & 0xff){
					case '1':
						num = 1;
						break;
					case '2':
						num = 2;
						break;
					case '3':
						num = 3;
						break;
					case '4':
						num = 4;
						break;
					case '5':
						num = 5;
						break;
					case '6':
						num = 6;
						break;
					case '7':
						num = 7;
						break;
					case '8':
						num = 8;
						break;
					case '9':
						num = 9;
						break;
					default:
						break;
				
				}
				switch(i){
					case 0:
						startX = num;
						break;
					case 1:
						startY = num;
						break;
					case 2:
						endX = num;
						break;
					case 3:
						endY = num;
						break;

				}

				if(ret == 0){
					break;
				}
			}
			

			
		}
		printf("#%ld put request \n", tid);
		printf("%d %d %d %d\n", startX, startY, endX, endY);
		requestNum++;
		taskPriority = requestNum;
		taskRank = requestNum;
		enqueue_Node(requestList, taskPriority, taskRank, bufStr, startX, startY, endX, endY);
		// scanf("%d %d %d %d", &startX, &startY, &endX, &endY);
		
		/*
		for(i=0; i<20; i++)
			enqueue_Node(requestList, taskPriority, taskRank, bufStr, startX+i, startY, endX, endY);
		*/
		

	}
	printf("#%ld end \n", tid);
	pthread_exit(NULL);
	
}

void *threadUberDriving(void *threadid)
{
	
	// print path on LCD
	long tid;
	tid = (long)threadid;
	printf("#%ld start \n", tid);
	
	int i = 0;	
	char str[bufSize];
	int nowX, nowY, sX, sY, eX, eY;
	
	int requestNum = 0;
	int taskPriority = 0;
	int taskRank = 0;
	char bufStr[] = "UBER";
	lcd_write_info_t display; 
	

	printf("#%ld serve request \n", tid);

	dequeue_Node(uberList, str);

	// if(nowRequest == NULL)
	//	printf("#3 NULL \n");
	
	// nowX = nowRequest->startX;
	// nowY = nowRequest->startY;
	sX = nowRequest->startX;
	sY = nowRequest->startY;
	eX = nowRequest->endX;
	eY = nowRequest->endY;
	serveRequest = 1;	

	
	
	

	if(sX != eX && sY != eY){
		sprintf(str, "(%d, %d)-->(%d, %d) ==> (%d, %d)-->(%d, %d)-->(%d, %d)\n", sX, sY, eX, eY, sX, sY, eX, sY, eX, eY);
	}
	else if (sX != eX || sY != eY){
		sprintf(str, "(%d, %d)-->(%d, %d) ==> (%d, %d)-->(%d, %d)\n", sX, sY, eX, eY, sX, sY, eX, eY);
	}
	else {
		sprintf(str, "(%d, %d)-->(%d, %d) ==> (%d, %d)\n", sX, sY, eX, eY, eX, eY);
	}
	
	printf("%s", str);
	

	// clear LCD
	ioctl(fd, LCD_IOCTL_CLEAR, NULL);
	// save output string
	display.Count = sprintf((char *)display.Msg, str);
	// print
	ioctl(fd, LCD_IOCTL_WRITE, &display);
	
	/*
	int t = rand() % 10;
	sleep(t);
	*/	
	enqueue_Node(uberList, taskPriority, taskRank, bufStr, eX, eY, 0, 0);
	
	print_LinkedList(uberList);
	printf("#%ld end \n", tid);

	pthread_exit(NULL);
	
}

int main(int argc, char *argv[])
{
	pthread_t thread_uber, thread_customer;
	pthread_t thread_driver;
	
	srand(time(NULL));
	int rc;
	int i, j, t;
	requestList = new_LinkedList();
	uberList = new_LinkedList();
	
    	if((fd = open("/dev/lcd", O_RDWR)) < 0)
    	{
       		printf("open /dev/lcd error \n");
        	return (-1);
    	}
	
	printf("In main(): creating thread\n");
	
	t = 1;
	rc = pthread_create(&thread_uber, NULL, threadUberQueue, (void *)t);	
	if(rc){
		printf("ERROR: return code from pthread create() is %d\n", rc);
		exit(-1);	
	}

	t = 2;
	rc = pthread_create(&thread_customer, NULL, threadCustomerRequest, (void *)t);	
	if(rc){
		printf("ERROR: return code from pthread create() is %d\n", rc);
		exit(-1);	
	}
		
	t = 3;
	while(1){
		if(getRequest){

			// 3. check there is a UBER can serve request
			//    if no UBER then wait

			while(1){
				ListNode *now;
				now = uberList->head;
				if(now != NULL){
					break;
				}
			}
			pthread_create(&thread_driver, NULL, threadUberDriving, (void *)t);
			getRequest = 0;
		}
			
	}
	
	close(fd);

	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);
}
