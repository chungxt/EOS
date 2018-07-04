#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define nameSize 20

typedef struct ListNode_Struct {
	int taskPriority;
	int taskRank;
	char taskName[nameSize];
	int startX, startY;
	int endX, endY;
	
	struct ListNode_Struct *successor;
} ListNode;

typedef struct LinkedList_Struct {
	ListNode *head;
	ListNode *tail;
} LinkedList;


LinkedList* new_LinkedList() { // new a linked list
	
	LinkedList *list;

	list = (LinkedList*) malloc(sizeof(LinkedList));

	list->head = NULL;
	list->tail = NULL;
	
	return list;	
}

void delete_LinkedList (LinkedList *list) { // delete a linked list (free all nodes)
	ListNode *now, *next;

	now = list->head;
	while (now != NULL){
		next = now->successor;
		free(now);
		now = next;
	}
	
	free(list);
}

void enqueue_Node(LinkedList *list ,int pri, int rank, const char* tsk, int startX, int startY, int endX, int endY) { // include sort & enqueue

	ListNode *new_node;
	ListNode *now;
	new_node = (ListNode*) malloc(sizeof(ListNode));
	
	new_node->taskPriority = pri;
	new_node->taskRank = rank;

	strcpy(new_node->taskName, tsk);

	new_node->startX = startX;
	new_node->startY = startY;
	new_node->endX = endX;
	new_node->endY = endY;

	new_node->successor = NULL;

	if(list->head == NULL){
		//printf("0\n");
		list->head = new_node;
		list->tail = new_node; 
	}
	else {


		int end = 0;
		now = list->head;
		while (1){
			if(now == list->head && now == list->tail){	//only one node in linkedlist
				
				if (now->taskRank > new_node->taskRank) { // insert at first
					//printf("1_1\n");
					new_node->successor = now;
					list->head = new_node;
				}
				else {						// insert at second
					//printf("1_2\n");
					list->tail->successor = new_node;
					list->tail = new_node;
					
				}

				end = 1;
			}
			else if(now == list->head){
				
				if (now->taskRank > new_node->taskRank) { // insert at first
					//printf("2_1\n");
					new_node->successor = now;
					list->head = new_node;
					end = 1;
				}
				if( (now->taskRank <= new_node->taskRank) && (now->successor->taskRank > new_node->taskRank) ) {
					//printf("2_2\n");					
					new_node->successor = now->successor;				
					now->successor = new_node;
					end = 1;
				}
				else { // still run
					//printf("2_3\n");
					now = now->successor;
				}

				
			}
			else if(now == list->tail){	// insert at last
				//printf("3_1\n");
				list->tail->successor = new_node;
				list->tail = new_node;
				
				end = 1;
			}
			else{
				//printf("4_1\n");			
				if( (now->taskRank <= new_node->taskRank) && (now->successor->taskRank > new_node->taskRank) ) {
					new_node->successor = now->successor;				
					now->successor = new_node;
					end = 1;
				}
				now = now->successor;
				
			}

			if(end == 1)
				break;			
			
		}

		
	}

}

int dequeue_Node( LinkedList* list, char str[]) { // pop order by taskPriority

	ListNode *now, *next;

	now = list->head;
	int tp;
	
	if (now != NULL){
		tp = now->taskPriority;
		sprintf(str, "\t=> pop the highest task => task name: %s, task priority: %d, task rank: %d (%d, %d) -> (%d, %d) \n", now->taskName, now->taskPriority, now->taskRank, now->startX, now->startY, now->endX, now->endY);
		//printf("%s", str);		
		list->head = now->successor;
		free(now);
		
	}
	return tp;

}

void print_LinkedList (LinkedList *list) { // print this linked list
	ListNode *now;
	int count = 0;
	now = list->head;
	while (now != NULL){
		printf("%d:\ttask name: %s,\ttask priority: %d,\ttask rank: %d (%d, %d) -> (%d, %d) \n", count, now->taskName, now->taskPriority, now->taskRank, now->startX, now->startY, now->endX, now->endY);	

		now = now->successor;
		count++;
	}
	

}

