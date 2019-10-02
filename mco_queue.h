#ifndef _MCO_QUEUE_H_
#define _MCO_QUEUE_H_

#include "mco.h"

// TODO: 현재는 queue 구현이다. deque 구현이 필요할 수 있다.
//       FIFO(first In first out) 구현임

struct mco_queue_node {
	void   *data;
	size_t data_len;
	
	struct mco_queue_node *prev;
	struct mco_queue_node *next;
};

struct mco_queue {
	int size;
	int size_max;
	
	struct mco_queue_node *head;
	struct mco_queue_node *tail;

	mco_data_struct_node_free node_free; // use it if you need to addtional free
};

struct mco_queue * mco_queue_create(mco_data_struct_node_free free, int max); // max(-1) is unlimit
void mco_queue_free(struct mco_queue **queue);
void mco_queue_node_free(struct mco_queue *queue, struct mco_queue_node **node);

int mco_queue_push(struct mco_queue *queue, const void *data, size_t data_len);
void * mco_queue_pop(struct mco_queue *queue);
int mco_queue_size(struct mco_queue *queue);

#endif
