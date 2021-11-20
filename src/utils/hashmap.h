#ifndef CANARY_HASHMAP_H
#define CANARY_HASHMAP_H
#define NUMBER_OF_CACHED_NODES 500
#include <stdio.h>
#include <string.h>
#include "definitions.h"

unsigned long counter=0;


typedef struct canary_node{
	char* path;
	int is_valid;
	int object_type;
	int id;
} canary_node;

// container to store the nodes
canary_node container[NUMBER_OF_CACHED_NODES];

void hashmap_init(){
	for(int i=0;i<NUMBER_OF_CACHED_NODES;i++){
		container[i].is_valid=0;
	}
}

// hashmap ops
// TODO: make the hashmap more efficient; For now it's linear. :(

// -1 if cache miss; 0 if doesn't exist; 1 if dir; 2 if file
int search_hashmap(char* path){
	for(int i=0;i<NUMBER_OF_CACHED_NODES;i++){
		if(container[i].is_valid && strcmp(path,container[i].path)==0){
			// update id
			container[i].id=(counter++);
			return container[i].object_type;
		}
	}
	return -1;
}


void push_object(const char* path, int object_type){
	// assert(strcmp(root_dirname, container.root_dirname)==0);
	int mini_indx=0;
	for(int i=0;i<NUMBER_OF_CACHED_NODES;i++){
		if(container[i].is_valid==0){
			container[i].object_type=object_type;
			container[i].path=strdup(path);
			container[i].id=(counter++);
			container[i].is_valid=1;
			return;
		}else if(container[mini_indx].id > container[i].id){
			mini_indx=i;
		}
	}
	// invalidate the one who was accessed least recently
	container[mini_indx].object_type=object_type;
	container[mini_indx].path=strdup(path);
	container[mini_indx].id=(counter++);
	container[mini_indx].is_valid=1;
}

void invalidate_object_if_exist(const char* path){
	printf("Trying to invalidate: %s\n", path);
	for(int i=0;i<NUMBER_OF_CACHED_NODES;i++){
		if(container[i].is_valid) printf("%s", container[i].path);
		if(container[i].is_valid && strcmp(path,container[i].path)==0){
			container[i].is_valid=0;
			printf("INVALIDATE SUCCESSFUL\n");
			return;
		}
	}
}


#endif
