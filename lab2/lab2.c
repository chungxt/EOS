#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.c"
#define readFileName "/lab2/data.txt"
#define writeFileName "/lab2/result.txt"
#define bufSize 20

int main () {

	LinkedList *list = new_LinkedList();
	int back = 0;
	clock_t sT, eT;
	double tT = 0;
	
	// get >10000 number
	
	FILE *fp;
	fp = fopen(readFileName, "r");	
	
	char *endPos;
	
	if(fp == NULL){
		printf("File open error.\n");
	}
	else{

	
		while(!feof(fp)){
						
			int taskPriority;
			fscanf(fp, "%d\n", &taskPriority);
			//printf("%d\n", taskPriority);

			char bufStr[bufSize];
			sprintf(bufStr, "%d", taskPriority);
			enqueue_Node(list, taskPriority, bufStr);
			
		}
		
		
		fclose(fp);

	}

	// end of file



	fp = fopen(writeFileName, "w");
	
	
	srand(time(NULL));
	
	if(fp == NULL){
		printf("File open error.\n");
	}
	else{
		
		// print ORIGINAL
		ListNode *now;
		int count = 0;
		
		now = list->head;
		fprintf(fp, "============== ORIGINAL PRIORITY TASK QUEUE ==============\n");

		while (now != NULL){
			fprintf(fp, "%d:\ttask name: %s,\ttask priority: %d\n", count, now->taskName, now->taskPriority);		
			now = now->successor;
			count++;
		}

		// enqueue a task
		int taskPriority = (rand() % 15000) + 1;

		sT = clock();
		enqueue_Node(list, taskPriority, "TEST TASK");
		eT = clock();

		tT = (double) (eT-sT)/CLOCKS_PER_SEC;
		fprintf(fp, "======= enqueue new task \"TEST TASK\" totally use %lf second =======\n", tT);

		// dequeue the highest priority task
		dequeue_Node(list);
		fprintf(fp, "======= dequeue the highest priority task =======\n");

		// print NOW
		count = 0;
		now = list->head;
		fprintf(fp, "============== NOW PRIORITY TASK QUEUE ==============\n");

		while (now != NULL){
			fprintf(fp, "%d:\ttask name: %s,\ttask priority: %d\n", count, now->taskName, now->taskPriority);		
			now = now->successor;
			count++;
		}
		// free memory
		delete_LinkedList(list);
		
		fclose(fp);

	}


// for input use 
/*
	while(1){

		int option = 0;
		char taskName[bufSize] = "";
		int taskPriority = 0;
		printf("\n===== priority queue info =====\n");
		printf("1: enqueue task\n2: dequeue task\n3: print all enqueue info\n4: exit this program\n");
		printf("please select options: ");
		scanf("%d", &option);
		
		
		switch(option){
			case 1:

				printf("please input task name:"); // enqueue
				scanf("%s", taskName);
				printf("please input task priority:");
				scanf("%d", &taskPriority);

				sT = clock();
				enqueue_Node(list, taskPriority, taskName);
				eT = clock();

				tT = (double) (eT-sT)/CLOCKS_PER_SEC;
				printf("======= totally use %lf second =======\n", tT);
				break;
			case 2:
				dequeue_Node(list);		// dequeue
				break;
			case 3:
				print_LinkedList(list);
				break;
			case 4:
				delete_LinkedList(list);
				back = 1;
						
			default:
				break;
		}
		
		if(back == 1)
			break;
	}
*/


	return 0;
}
