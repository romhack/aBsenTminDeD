#pragma once
#ifndef _PQUEUE_H
#define _PQUEUE_H

#include "encode.h"

//We modify the data type to hold pointers to Huffman tree nodes


#define MAX_SZ 0x100

typedef struct _pQueueNode {
	htNode * val;
	unsigned int priority;
	struct _pQueueNode *next;
}pQueueNode;

typedef struct _pQueue {
	size_t size;
	pQueueNode *first;
}pQueue;

void initPQueue(pQueue **queue);
void addPQueue(pQueue **queue, htNode * val, unsigned int priority);//add queue node with sorting based on priority
void addPQueue_LI(pQueue **queue, htNode * val, int direction, int parental_node);//add node without sorting - simple filo queue
htNode * getPQueue_FO(pQueue **queue);

#endif