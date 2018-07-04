/* 
 * ubers.c
 */
 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

#include "LinkedList.c"

#define UBERS_NUM 4
//#define REQUEST_NUM 7
#define MAP_SIZE 9
#define bufSize 200



LinkedList *requestList = NULL;
LinkedList *uberList = NULL;
ListNode *nowRequest = NULL;
sem_t sem_getR;
sem_t sem_waitR;
sem_t sem_serveR;
// 1. control flow
// 2. protect global variable	

typedef struct Map_Struc {
	int waiting_num;	// how many cars on the location
	int cond_index;		// how many cars on the location
	int is_using;		// the location has uber using or not
} Map;
Map UMAP[MAP_SIZE][MAP_SIZE];
LinkedList *condIndexList = NULL;	// store cond index list
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond[UBERS_NUM];		// set N condition for N car to serve multiple request
int thread_num = 0;
int fd;
unsigned short data_led;
//_7seg_info_t    data_7seg;

void *threadUberQueue(void *threadid)
{

	// check list number != 0

	long tid;
	tid = (long)threadid;
	//printf("#%ld start \n", tid);

	int i = 0, j = 0, k = 0;
	int x, y;
	char str[bufSize];

	int taskPriority = 0;
	int taskRank = 0;


	for(j = 0; j < MAP_SIZE; j++){
		for(k = 0; k < MAP_SIZE; k++){
		
			UMAP[j][k].waiting_num = 0;
			UMAP[j][k].cond_index = -1;
			UMAP[j][k].is_using = 0;
		}
	}

	srand(time(NULL));
	
	
	while(i < UBERS_NUM){
		x = rand() % MAP_SIZE + 1;
		y = rand() % MAP_SIZE + 1;
		
		enqueue_Node(uberList, taskPriority, taskRank, "UBER", x, y, 0, 0);
		
		//printf("(%d, %d) ", x, y);

		// BTW do init of mutex

		pthread_cond_init(&cond[i], NULL);
		enqueue_Node(condIndexList, i, taskRank, "COND", 0, 0, 0, 0); // 0 1 2 3
		i++;
	}
	//printf("\n");
	
	//print_LinkedList(uberList);

	while(1){
		ListNode *now;
		now = requestList->head;
		if(now != NULL){
			//printf("#%ld get request \n", tid);
			nowRequest = now;
			sem_post(&sem_getR);
			//printf("post sem_getR\n");

			// 1. confirm driving thread has been create
			// 2. wait for correct nowX, nowY 
			// (especially multiple requests)
			
			
			sem_wait(&sem_waitR);
			//printf("wait sem_waitR\n");

			sem_wait(&sem_serveR);
			//printf("wait sem_serveR\n");
				
					
				
			dequeue_Node(requestList, str);
			printf("\t>> REQUEST SERVE \n");
			//print_LinkedList(requestList);
			// break;
			
			
		}
	}
	//printf("#%ld end \n", tid);
	pthread_exit(NULL);
	
}



void *threadCustomerRequest(void *threadid)
{
	// get num from keypad
	// check if button is clicked for 4 times (use loop), then list enqueue
	long tid;
	tid = (long)threadid;
	//printf("#%ld start \n", tid);
		
	int taskPriority = 0;
	int taskRank = 0;

	//int startX[UBERS_NUM], startY[UBERS_NUM], endX[UBERS_NUM], endY[UBERS_NUM];
	int startX, startY, endX, endY;
	int i = 0, j = 0;
	int ret;

	enqueue_Node(requestList, taskPriority, taskRank, "REQUEST", 1, 1, 2, 1);
	enqueue_Node(requestList, taskPriority, taskRank, "REQUEST", 2, 1, 2, 2);
	enqueue_Node(requestList, taskPriority, taskRank, "REQUEST", 5, 5, 2, 5);
	enqueue_Node(requestList, taskPriority, taskRank, "REQUEST", 7, 1, 2, 7);
	enqueue_Node(requestList, taskPriority, taskRank, "REQUEST", 3, 5, 2, 3);
	enqueue_Node(requestList, taskPriority, taskRank, "REQUEST", 8, 5, 2, 8);
	enqueue_Node(requestList, taskPriority, taskRank, "REQUEST", 4, 6, 2, 4);

	while(1){

		scanf("%d %d %d %d", &startX, &startY, &endX, &endY);
		
		enqueue_Node(requestList, taskPriority, taskRank, "REQUEST", startX, startY, endX, endY);
		
		

	}
	//printf("#%ld end \n", tid);
	
	pthread_exit(NULL);
	
}

void *threadUberDriving(void *threadid)
{
	
	// print path on LCD
	long tid;
	tid = (long)threadid;
	//printf("#%ld start \n", tid);
	
	//int i = 0;	
	char str[bufSize], str1[bufSize], str2[bufSize]="";
	int nowX, nowY, sX, sY, eX, eY;
	
	int taskPriority = 0;
	int taskRank = 0;
	int condIndex = -1;

	int shmid;
	key_t key;
	char *shm, *s;
	
	key = 5678;
	
	if((shmid = shmget(key, bufSize, 0666)) < 0){
		perror("shmget");
		exit(1);
	}
	
	if((shm = shmat(shmid, NULL, 0)) == (char *) -1){
		perror("shmat");
		exit(1);
	}
	printf("client attach the share memory created by server\n");
	
	
	



	
	// protect position
	pthread_mutex_lock(&mut);

	//printf("#%ld serve request \n", tid);

	dequeue_Node(uberList, str);

	// if(nowRequest == NULL)
	//	printf("#3 NULL \n");
	
	sX = nowRequest->startX;
	sY = nowRequest->startY;
	eX = nowRequest->endX;
	eY = nowRequest->endY;

	sem_post(&sem_serveR);
	//printf("post sem_serveR\n");

	UMAP[eX][sY].waiting_num = UMAP[eX][sY].waiting_num + 1;
	//printf("\t%d %d %d here\n", sX, sY, UMAP[eX][sY].waiting_num);	
	if(UMAP[eX][sY].waiting_num > 1){

		while(UMAP[eX][sY].is_using == 1){
			// get cond index from map
			condIndex = UMAP[eX][sY].cond_index;
			printf("\t>> %d %d lock by %d \n", sX, sY, condIndex);

			// LED BLINKS!
			/*
			int i;
			for(i = 0; i < 1; i++){
				// turn on D9 
				data_led = LED_D9_INDEX;
				ioctl(fd, LED_IOCTL_BIT_SET, &data_led);
				//printf("Turn on D9 \n");
				sleep(1);

				// turn off D9
				data_led = LED_D9_INDEX;
				ioctl(fd, LED_IOCTL_BIT_CLEAR, &data_led);
				//printf("Turn off D9 \n");
				sleep(1);
			}
			*/

			sprintf(str2, "wait");
			pthread_cond_wait(&cond[condIndex], &mut);
			//printf("HEY %d\n", UMAP[eX][sY].waiting_num);
		}
	}
	else if(UMAP[eX][sY].waiting_num == 1){
		// set cond index to cond map
		
		condIndex = dequeue_Node(condIndexList, str1);
		printf("\t>> %d %d protect by %d \n", sX, sY, condIndex);
		UMAP[eX][sY].cond_index = condIndex;
		//printf("HEY %d\n", UMAP[eX][sY].waiting_num);
	}
	UMAP[eX][sY].is_using = 1;
	thread_num = thread_num + 1;
	printf("\nthread num %d \n\n", thread_num);

	// 7-SEG!
	/*
	ioctl(fd, _7SEG_IOCTL_ON, NULL);


	data_7seg.Mode = _7SEG_MODE_HEX_VALUE;
	data_7seg.Which = _7SEG_ALL;
	data_7seg.Value = thread_num;    // change to thread_num
	ioctl(fd, _7SEG_IOCTL_SET, &data_7seg);
	//sleep(1);
	*/

	pthread_mutex_unlock(&mut);
	int distance = abs(eX-sX) + abs(eY-sY);

	if(sX != eX && sY != eY){
		sprintf(str, "(%d, %d)-->(%d, %d): %d [(%d, %d)-%s->(%d, %d)-->(%d, %d)]\n", sX, sY, eX, eY, distance, sX, sY, str2, eX, sY, eX, eY);
	}
	else if (sX != eX || sY != eY){
		sprintf(str, "(%d, %d)-->(%d, %d): %d [(%d, %d)-%s->(%d, %d)]\n", sX, sY, eX, eY, distance, sX, sY, str2, eX, eY);
	}
	else {
		sprintf(str, "(%d, %d)-->(%d, %d): %d [(%d, %d)-%s-]\n", sX, sY, eX, eY, distance, eX, eY, str2);
	}
		
	
	
	
	printf("client write path info to the share memory\n");
	strcat(shm, str);
	
	printf("client detach the share memory\n");
	shmdt(shm);
	
	printf("%s", str);


	int secs = rand() % 10;
	secs = 5;
	while(secs){
		printf("\t%d %d ...doodle... %d %d, %d secs\n", sX, sY, eX, sY, secs--);
		sleep(1);
	}
	

	// protect position
	pthread_mutex_lock(&mut);
	
	enqueue_Node(uberList, taskPriority, taskRank, "UBER", eX, eY, 0, 0);
	printf("\t>> UBER BACK \n");
	//print_LinkedList(uberList);
	//printf("#%ld end \n", tid);

	
	if(UMAP[eX][sY].waiting_num > 1){
		pthread_cond_signal(&cond[condIndex]);
		
		
	}
	UMAP[eX][sY].is_using = 0;
	UMAP[eX][sY].waiting_num = UMAP[eX][sY].waiting_num - 1;
	if(UMAP[eX][sY].waiting_num == 0){
		// set cond index to cond map
		enqueue_Node(condIndexList, condIndex, taskRank, "COND", 0, 0, 0, 0);
		
		UMAP[eX][sY].cond_index = -1;
		
		printf("\t>> %d %d %d release here %d\n", sX, sY, UMAP[eX][sY].waiting_num, condIndex);
		print_LinkedList(condIndexList);	
	}
	
	
	thread_num = thread_num - 1;

	printf("\nthread num %d \n\n", thread_num);
	// 7-SEG!
	/*
	ioctl(fd, _7SEG_IOCTL_ON, NULL);


	data_7seg.Mode = _7SEG_MODE_HEX_VALUE;
	data_7seg.Which = _7SEG_ALL;
	data_7seg.Value = thread_num;    // change to thread_num
	ioctl(fd, _7SEG_IOCTL_SET, &data_7seg);
	//sleep(1);
	*/
	pthread_mutex_unlock(&mut);

	pthread_exit(NULL);
	
}

void ParentProcess()
{

	char c;
	int shmid;
	key_t key;
	char *shm, *s;
	int retval;
	key = 5678;
	
	if((shmid = shmget(key, bufSize, IPC_CREAT | 0666)) < 0){
		perror("shmget");
		exit(1);
	}
	if((shm = shmat(shmid, NULL, 0)) == (char *) -1){
		perror("shmat");
		exit(1);
	}
	printf("server create and attach the share memory\n");
	
	printf("waiting other process write the share memory...\n");
	
	printf("server read characters from share memory...\n");

	while(1){
		sleep(60);
		printf("control center\n");
		/*for(s = shm; *s != '\0'; s++)
			putchar(*s);
		putchar('\n');*/
		printf(" => %s\n", shm);
		*shm = '\0';
	}
	
	shmdt(shm);
	
	printf("server destroy the share memory\n");
	retval = shmctl(shmid, IPC_RMID, NULL);
	if(retval < 0){
		fprintf(stderr, "server remove share memory failed\n");
		exit(1);
	}
	
}

int main(int argc, char *argv[])
{

	pid_t cpid;
	
	
	
	cpid = fork();
	if(cpid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	
	if(cpid == 0){ // child
	
		pthread_t thread_uber, thread_customer;
		pthread_t thread_driver;
	
		int res;
	    	pthread_t a_thread;
		res = sem_init(&sem_getR, 0, 0);
		res = sem_init(&sem_waitR, 0, 0);
		res = sem_init(&sem_serveR, 0, 0);
		if (res != 0)
	    	{
			perror("Semaphore initialization failed");
	    	}

		printf("START\n");

		srand(time(NULL));
		int rc;
		int i, j, t;
		requestList = new_LinkedList();
		uberList = new_LinkedList();
		condIndexList = new_LinkedList();

		/*
		if((fd = open("/dev/lcd", O_RDWR)) < 0)
	    	{
	       		printf("open /dev/lcd error \n");
			return (-1);
	    	}
		*/	
	
		//printf("In main(): creating thread\n");
	
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
			sem_wait(&sem_getR);
			//printf("wait sem_getR\n");

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
			sem_post(&sem_waitR);
			//printf("post sem_waitR\n");
		
			
		}
	
		close(fd);

		printf("Main: program completed. Exiting.\n");
		pthread_exit(NULL);
	}
	else{ // parent
		ParentProcess();
	}
		
	
}

