/**
 * @file    mco_map.c
 * @brief   MAP 자료구조 구현체
 * @date    2019.09.20
 * @author  naze@dreamsecurity.com
 *
 */

#include "mco_map.h"
#include "mco_inter.h"

/* internal function */
static size_t generate_map_key(const char *key)
{
	size_t i, map_key = 0, key_len = strlen(key);

	for ( i = 0; i < key_len; i++ ) {
		map_key += key[i];
	}

	return map_key;
}

struct mco_map * mco_map_create(mco_data_struct_node_free node_free)
{
	struct mco_map *map = NULL;

	if ( !(map = (struct mco_map *)calloc(1, sizeof(struct mco_map))) ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}

	map->node_free = node_free;

	return map;

out:
	safe_free(map);
	
	return NULL;
}

void mco_map_free(struct mco_map **map)
{
	if ( map ) {
		if ( *map ) {
			struct mco_map_node *node, *node_next;

			for ( node = (*map)->head; node; node = node_next ) {
				node_next = node->next;

				if ( (*map)->node_free ) {
					(*map)->node_free(node->data, node->data_len);
				}
				safe_free(node);
			}
			safe_free(*map);
		}
	}
}
	
int mco_map_push_back(struct mco_map *map, const char *key, const void *data, size_t data_len)
{
	struct mco_map_node *node = NULL;

	if ( !map || !key || !data ) {
		MCO_SET_ERROR("invalid argument - map: %s, key: %s, data: %s", map ? "valid" : "null", key ? "valid" : "null", data ? "valid" : "null");
		goto out;
	}

	// prevent to insert duplication node(key)
	if ( mco_map_find(map, key) ) return 1;
	
	if ( !(node = calloc(1, sizeof(struct mco_map_node))) ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}
	
	if ( !(node->data = calloc(1, data_len)) ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}

	// assign to node data from arguments
	node->key = generate_map_key(key);
	memcpy(node->data, data, data_len);
	node->data_len = data_len;

	// insert to list
	node->prev = NULL;
	node->next = NULL;

	if ( map->size == 0 ) {
		map->head = map->tail = node;
	}
	else if ( map->size == 1 ) {
		node->prev = map->head;
		map->head->next = node;
		map->tail = node;
	}
	else {
		node->prev = map->tail;
		map->tail->next = node;
		map->tail = node;
	}
	
	map->size++;

	return 0;

out:
	if ( node ) {
		safe_free(node->data);
		safe_free(node);
	}

	return -1;
}
	
void * mco_map_find(struct mco_map *map, const char *key)
{
	if ( !map || !key ) {
		MCO_SET_ERROR("invalid argument - map: %s, key: %s", map ? "valid" : "null", key ? "valid" : "null");
		return NULL;
	}

	size_t find_key = generate_map_key(key);
	for ( struct mco_map_node *node = map->head; node; node = node->next ) {
		if ( node->key == find_key ) {
			return node->data; // found
		}
	}

	return NULL; // can not found
}

int mco_map_erase(struct mco_map *map, const char *key)
{
	if ( !map || !key ) {
		MCO_SET_ERROR("invalid argument - map: %s, key: %s", map ? "valid" : "null", key ? "valid" : "null");
		return -1;
	}

	struct mco_map_node *node, *node_next;
	size_t find_key = generate_map_key(key);
	for ( node = map->head; node; node = node_next ) {
		node_next = node->next;

		if ( node->key == find_key ) {
			if ( map->size == 1 ) {
				map->head = map->tail = NULL;
			}
			else if ( map->size == 2 ) {
				if ( node == map->head ) {
					map->tail->prev = NULL;
					map->head = map->tail;
				}
				else if ( node == map->tail ) {
					map->head->next = NULL;
					map->tail = NULL;
				}
				else {
					MCO_SET_ERROR("internal map error: invalid node pointer");
					return -1;
				}
			}
			else if ( map->size > 2 ) {
				if ( node == map->head ) {
					node->next->prev = NULL;
					map->head = node->next;
				}
				else if ( node == map->tail ) {
					node->prev->next = NULL;
					map->tail = node->prev;
				}
				else {
					node->prev->next = node->next;
					node->next->prev = node->prev;
				}
			}
			else {
				MCO_SET_ERROR("internal map error: invalid map size: %d", map->size);
				return -1;
			}

			if ( map->node_free ) {
				map->node_free(node->data, node->data_len);
			}
			safe_free(node);
			
			map->size--;

			return 0;
		}
	}

	MCO_SET_ERROR("can not found: key - %s", key);

	return 1; // can not found
}
