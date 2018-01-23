#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 500

typedef struct 
{
	char buffer[BUFFER_SIZE];
	int16_t in;
	int16_t out;

	int16_t __size;
} Queue;


void QueueInit(Queue* q);

void QueuePut(Queue* q, char c);

void QueuePutArray(Queue* q, char* data, int len);

void QueuePutString(Queue* q, char* string);

int QueueStrCmp(Queue* q, char* string);

int QueueSize(Queue* q);

char QueueGet(Queue* q);

char QueuePeekOffset(Queue* q, uint32_t offset);

void QueuePopFront(Queue* q);

void QueuePop(Queue* q, uint32_t len);

bool QueueIsEmpty(Queue* q);

bool QueueIsFull(Queue* q);

char* QueueGetPtr(Queue* q);

int QueueGetUpTo(Queue* q, char* buffer, int max);

void QueueClear(Queue* q);

#endif