#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>

typedef struct Node {
	void* data;
	struct Node *next;
} Node;

typedef struct Queue {
	int size;
	Node* front;
	Node* rear;
} Queue;

void enqueue(Queue* q, void* data);
void *dequeue(Queue* q);
void clearQueue(Queue* q);
bool hasNext(Queue* q);

#endif