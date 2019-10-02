#ifndef _MCO_MAP_H_
#define _MCO_MAP_H_

#include "mco.h"

struct mco_map_node {
	size_t key;
	void   *data;
	size_t data_len;
	
	struct mco_map_node *prev;
	struct mco_map_node *next;
};

struct mco_map {
	int size;
	
	struct mco_map_node *head;
	struct mco_map_node *tail;

	mco_data_struct_node_free node_free; // use it if you need to addtional free
};

struct mco_map * mco_map_create(mco_data_struct_node_free free);
void mco_map_free(struct mco_map **map);

int mco_map_push_back(struct mco_map *map, const char *key, const void *data, size_t data_len);
void * mco_map_find(struct mco_map *map, const char *key);
int mco_map_erase(struct mco_map *map, const char *key);

#endif
