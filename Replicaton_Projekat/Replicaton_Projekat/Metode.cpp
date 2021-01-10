#include "Header1.h"


CRITICAL_SECTION cs; //Globalna promenljiva, kriticna sekcija

queue* red_proxy;
redRedova* red_redova;


struct queue* queueCreate()
{

	struct queue* q;

	q = (queue*)malloc(sizeof(struct queue));

	q->head = q->tail = 0;

	return q;
}


void enq(struct queue* q, char* value)
{
	struct elt* e;

	e = (elt*)malloc(sizeof(struct elt));
	assert(e);

	e->value = value;

	/* Because I will be the tail, nobody is behind me */
	e->next = 0;

	if (q->head == 0) {
		/* If the queue was empty, I become the head */
		q->head = e;
	}
	else {
		/* Otherwise I get in line after the old tail */
		q->tail->next = e;
	}

	/* I become the new tail */
	q->tail = e;
}


int queueEmpty(const struct queue* q)
{
	return (q->head == 0 || q->head == NULL);
}


/* remove and return value from front of queue */
void deq(struct queue* q)
{
	struct elt* e;

	assert(!queueEmpty(q));

	/* patch out first element */
	e = q->head;
	q->head = q->head->next;

	free(e);

}

void queueDestroy(struct queue* q)
{
	while (!queueEmpty(q)) {
		deq(q);
	}

	free(q);
	q = NULL;
}

void queuePrint(struct queue* q)
{
	struct elt* e;
	int brojac = 0;
	for (e = q->head; e != 0; e = e->next) {
		brojac++;
	}
	printf("%d", brojac);
	putchar('\n');
}


void ListAdd(int number, SOCKET s, DWORD id, HANDLE h, List** head)
{
	List* el;
	el = (List*)malloc(sizeof(List));
	el->num = number;
	el->s = s;
	el->threadID = id;
	el->clienth = h;
	el->next = NULL;
	el->ready = 0;
	el->ugasi = 0;
	el->brojizabranihservisa = 0;
	if (*head == NULL) {
		*head = el;
	}
	else {
		List* temp = *head;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = el;
	}
}


int ListCount(List* head)
{
	List* temp = head;
	int ret = 0;
	while (temp) {
		ret++;
		temp = temp->next;
	}
	return ret;
}


List* ListElementAt(int index, List* head)
{
	if (ListCount(head) > 0) {
		List* temp = head;

		while (temp->num != index) {
			temp = temp->next;
		}
		return temp;
	}
	return NULL;
}