#include "queue.h"
#include <string.h>

void QueueInit(Queue* q) 
{
	for (int i = 0; i < BUFFER_SIZE; ++i)
		q->buffer[i] = 0;

	q->in = 0;
	q->out = 0;
	q->__size = 0;
}

void QueuePutArray(Queue* q, char* data, int len)
{
	for (int i = 0; i < len; ++i)
	{
		QueuePut(q, data[i]);
	}
}

void QueuePutString(Queue* q, char* string)
{
	while (*string)
	{
		QueuePut(q, *string);
		++string;
	}
}

int QueueStrCmp(Queue* q, char* string)
{
	uint32_t len = strlen(string);
	if (q->__size < len)
		return 0;

	for (uint32_t i = 0; i < len; ++i)
	{
		if (QueuePeekOffset(q, i) != string[i])
			return 0;
	}

	QueuePop(q, len);

	return 1;
}

void QueuePut(Queue* q, char c) 
{
	if (q->__size == BUFFER_SIZE)
		return;

	q->buffer[q->in] = c;
	++q->in;
	++q->__size;

	if (q->in == BUFFER_SIZE) 
		q->in = 0;
}

char QueueGet(Queue* q) 
{
	return *QueueGetPtr(q);
}

char* QueueGetPtr(Queue* q)
{
	if (q->__size == 0)
		return 0;
	
	char* c = q->buffer + q->out;

	++q->out;
	--q->__size;

	if (q->out == BUFFER_SIZE)
		q->out = 0;

	return c;	
}

void QueuePop(Queue* q, uint32_t len)
{
	if (len < q->__size)
		len = q->__size;

	for (int i = 0; i < len; ++i)
		QueueGet(q);
}

bool QueueIsEmpty(Queue* q)
{
	return q->__size == 0;
}

bool QueueIsFull(Queue* q)
{
	return q->__size == BUFFER_SIZE;
}

char QueuePeekOffset(Queue* q, uint32_t offset)
{
	if (offset >= q->__size)
		return 0;

	return q->buffer[(q->out + offset) % BUFFER_SIZE];
}

void QueuePopFront(Queue* q)
{
	if (q->__size == 0)
		return;

	--q->in;
	--q->__size;

	if (q->out == -1)
		q->out = BUFFER_SIZE - 1;
}

int QueueSize(Queue* q)
{
	return q->__size;
}

int QueueGetUpTo(Queue* q, char* buffer, int max)
{
	int right = 0;

	if (q->out >= q->in)
		right = BUFFER_SIZE - q->out;
	else
		right = q->in - q->out;

	if (right > max)
		right = max;

	memcpy(buffer, q->buffer + q->out, right);

	q->out += right;
	q->__size -= right;
	if (q->out >= BUFFER_SIZE)
		q->out = 0;

	return right;
}

void QueueClear(Queue* q)
{
	q->in = 0;
	q->out = 0;
	q->__size = 0;
}