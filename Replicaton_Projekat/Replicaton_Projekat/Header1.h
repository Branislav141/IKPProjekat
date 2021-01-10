#pragma once

#include <ws2tcpip.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define NUMBER_OF_SERVICES 3
#define SERVER_SLEEP_TIME 50
#define DEFAULT_BUFLEN 150
#define MAX_SIZE 100
#define NUMBER_OF_SERVICES 3
#define DEFAULT_PORT "27016"
#define DEFAULT_PORT_SERVICE 27017
#pragma warning(disable:4996) 
#pragma comment(lib, "Ws2_32.lib")


typedef struct elt {
	struct elt* next;
	char* value;
}elt;

typedef struct queue {
	struct elt* head;  /* dequeue this next */
	struct elt* tail;  /* enqueue after this */
}queue;

typedef struct redRedova {
	struct queue* head;
	struct queue* tail;
};

typedef struct list{
	int num;
	SOCKET s;
	DWORD threadID;
	HANDLE clienth;
	int ugasi = 0;
	int ready;
	int brojizabranihservisa;
	struct drajver* drajver;
	struct list* next;
} List;


int queueEmpty(const struct queue* q);
void queuePrint(struct queue* q);
struct queue* queueCreate();

void enq(struct queue* q, char* value);
void queueDestroy(struct queue* q);
void deq(struct queue* q);

void ListAdd(int number, SOCKET s, DWORD id, HANDLE h, List** head);
int ListCount(List* head);
List* ListElementAt(int index, List* head);