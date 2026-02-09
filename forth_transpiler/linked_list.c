//
// Simple linked list implementation
//

#ifndef LINKED_LIST_C
#define LINKED_LIST_C

#include <stddef.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

typedef char byte;

typedef struct __linked_list_node_struct linked_list_node_t;
typedef struct __linked_list_struct linked_list_t;



struct __linked_list_node_struct{
	void* data;
	struct __linked_list_node_struct* next_node;
};


struct __linked_list_struct{
	struct __linked_list_node_struct* start_node;
	size_t size;
	void (*data_free_func)(void*);
};



#ifdef LINKED_LIST_EASY_NAMES
#define llist_create linked_list_create
#define llist_free linked_list_free
#define llist_append linked_list_append
#define llist_prepend linked_list_prepend
#define llist_get_at linked_list_get_at
#define llist_delete_at linked_list_delete_at
#define llist_foreach linked_list_foreach
#endif




linked_list_t* linked_list_create(){
	
	linked_list_t* list = (linked_list_t*)malloc(sizeof(linked_list_t));
	
	list->start_node = NULL;
	list->size = 0;
	list->data_free_func = NULL;
	
	return list;
}



void linked_list_free(linked_list_t* list){
	linked_list_node_t* curr_node = list->start_node;
	linked_list_node_t* next_node = 0;
	while((list->size--) > 0){
		next_node = curr_node->next_node;
		if(list->data_free_func != NULL) list->data_free_func(curr_node->data);
		
		free(curr_node);
		curr_node = next_node;
	}
	free(list);
}


byte linked_list_append(linked_list_t* list, void* data){
	
	linked_list_node_t* new_node = malloc(sizeof(linked_list_node_t));
	// Check if memory allocation for a new node failed
	if(new_node == NULL) return 1;
	
	new_node->next_node = NULL;
	new_node->data = data;	
	
	if(list->size == 0){
		list->start_node = new_node;
		list->size = 1;
		return 0;
	}
	
	
	linked_list_node_t* curr_node = list->start_node;
	size_t size_left = list->size;
	while((size_left--) > 1){
		curr_node = curr_node->next_node;
	}
	
	curr_node->next_node = new_node;
	
	list->size++;
	
	return 0;
};


byte linked_list_prepend(linked_list_t* list, void* data){
	linked_list_node_t* new_node = malloc(sizeof(linked_list_node_t));
	if(new_node == NULL) return 1;
	
	new_node->next_node = list->start_node;
	new_node->data = data;
	
	list->start_node = new_node;
	
	list->size++;
	
	return 0;
}


void* linked_list_get_at(linked_list_t* list, size_t index){
	if(index >= list->size) return NULL;
	
	size_t curr_index = 0;
	linked_list_node_t* curr_node = list->start_node;
	
	while(curr_index != index){
		curr_node = curr_node->next_node;
		curr_index += 1;
	}
	
	return curr_node->data;
}



byte linked_list_delete_at(linked_list_t* list, size_t index){
	if(index >= list->size) return 1;
	
	if(list->size == 1){
		if(list->data_free_func != NULL) list->data_free_func(list->start_node->data);
		free(list->start_node);
		list->start_node = NULL;
		list->size = 0;
	}
	
	size_t curr_index = 1;
	linked_list_node_t* prev_node = list->start_node;
	linked_list_node_t* curr_node = prev_node->next_node;
	
	while(curr_index != index){
		prev_node = curr_node;
		curr_node = prev_node->next_node;
		curr_index += 1;
	}
	
	// curr_node contains the node that is to be deleted
	// prev_node contains the node before curr_node, to link curr_node->next_node with the previous node
	
	prev_node->next_node = curr_node->next_node;
	
	list->size--;
	
	if(list->data_free_func != NULL) list->data_free_func(curr_node->data);
	
	free(curr_node);
	
	return 0;
}



void linked_list_foreach(linked_list_t *list, void (*data_func)(void *, void *), void *func_arg){
	size_t size_left = list->size;
	linked_list_node_t* curr_node = list->start_node;
	
	while((size_left--) > 0){
		data_func(curr_node->data, func_arg);
		curr_node = curr_node->next_node;
	}

	return;
}

#endif