#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "queue.h"

void enqueue(Queue* q, void* data) {
	Node* newNode = malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = NULL;
	
	if (q->rear) {
		q->rear->next = newNode;
	} else {
		q->front = newNode;
	}
	
	q->rear = newNode;
	q->size++;
}

void* dequeue(Queue* q) {
	if (q->front == NULL) {
		return NULL;
	}
	
	Node* frontNode = q->front;
	void* data = frontNode->data;
	
	q->front = frontNode->next;
	if (q->front == NULL) {
		q->rear = NULL;
	}
	
	free(frontNode);
	q->size--;
	
	return data;
}

void clearQueue(Queue* q) {
	Node* current = q->front;
	while (current) {
		Node *next = current->next;
		free(current);
		current = next;
	}
	
	q->front = NULL;
	q->rear = NULL;
	q->size = 0;
}

bool hasNext(Queue* q) {
	if (q->size == 0) {
		return false;
	}
	
	return true;
}