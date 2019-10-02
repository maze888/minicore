#include "mco_queue.h"
#include "mco_inter.h"

struct mco_queue * mco_queue_create(mco_data_struct_node_free node_free, int max)
{
	struct mco_queue *queue = NULL;
	
	queue = (struct mco_queue *)calloc(1, sizeof(struct mco_queue));
	if ( !queue ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}

	queue->node_free = node_free;
	queue->size_max = max;

	return queue;

out:
	safe_free(queue);

	return NULL;
}

void mco_queue_free(struct mco_queue **queue)
{
	if ( queue && *queue ) {
		struct mco_queue_node *node, *node_next;

		for ( node = (*queue)->head; node; node = node_next ) {
			node_next = node->next;

			if ( (*queue)->node_free ) {
				(*queue)->node_free(node->data, node->data_len);
			}
			else {
				safe_free(node->data);
			}
			safe_free(node);
		}
		safe_free(*queue);
	}
}

void mco_queue_node_free(struct mco_queue *queue, struct mco_queue_node **node)
{
	if ( queue && node && *node ) {
		if ( queue->node_free ) {
			queue->node_free((*node)->data, (*node)->data_len);
		}
		else {
			safe_free((*node)->data);
		}
		safe_free(*node);
	}
}

int mco_queue_push(struct mco_queue *queue, const void *data, size_t data_len)
{
	struct mco_queue_node *node = NULL;

	if ( !queue || !data || data_len == 0 ) {
		MCO_SET_ERROR("invalid argument - queue: %s, key: %s, data: %s, data_len: %d", queue ? "valid" : "null", data ? "valid" : "null", data_len);
		goto out;
	}

	if ( queue->size_max > 0 && queue->size >= queue->size_max ) {
		MCO_SET_ERROR("queue is full. curr: %d, max: %d", queue->size, queue->size_max);
		goto out;
	}
	
	if ( !(node = calloc(1, sizeof(struct mco_queue_node))) ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}
	
	if ( !(node->data = calloc(1, data_len)) ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}

	// now, node->prev:node->next is NULL
	switch (queue->size) {
		case 0:
			queue->head = queue->tail = node;
			break;
		case 1:
			queue->tail->next = node;
			queue->head = node->prev = queue->tail;
			queue->tail = node;
			break;
		default:
			queue->tail->next = node;
			queue->tail = node;
			node->prev = queue->tail;
	}

	queue->size++;

	return 0;

out:
	if ( node ) {
		safe_free(node->data);
		safe_free(node);
	}

	return -1;
}

void * mco_queue_pop(struct mco_queue *queue)
{
	struct mco_queue_node *node = NULL;

	if ( !queue ) {
		MCO_SET_ERROR("invalid argument - queue: %s", queue ? "valid" : "null");
		goto out;
	}
	
	switch (queue->size) {
		case 0:
			MCO_SET_ERROR("queue is empty");
			goto out;
		case 1:
			node = queue->head;
			queue->head = queue->tail = NULL;
			break;
		case 2:
			node = queue->head;
			queue->head = queue->head->next;
			queue->head->prev = queue->tail = NULL;
			break;
		default:
			node = queue->head;
			queue->head = queue->head->next;
			queue->head->prev = NULL;
	}
	
	queue->size--;

out:
	return node;
}

int mco_queue_size(struct mco_queue *queue)
{
	return queue ? queue->size : -1;
}
