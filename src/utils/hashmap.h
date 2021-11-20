#ifndef CANARY_HASHMAP_H
#define CANARY_HASHMAP_H

#include <stdio.h>
#include <string.h>
#include "definitions.h"


typedef struct canary_node{
	char* root_dirname; // like /, /bar, /foo
	char* filenames[100000]; // like hello.txt, name.txt etc
	char* dirs[100000]; // like bar, foo etc
	int number_of_files;
	int number_of_dirs;
	int is_valid;
} canary_node;

// container to store the nodes
canary_node container={
	.is_valid=0,
	.number_of_dirs=0,
	.number_of_files=0,
};
// container.is_valid=0;
// container.number_of_files=0;
// container.number_of_dirs=0;

// hashmap ops
// TODO: make the hashmap more efficient; For now it's linear. :(x

// 0 if doesn;t exist; 1 if dir; 2 if file
int search_hashmap(char* root_dirname, char* object_name){

	printf("FIND in %d dirs and %d files; root_dirname: %s, objectname: %s\n\n", container.number_of_dirs, container.number_of_files, root_dirname,  object_name);

	if(container.is_valid==0) return 0;
	// for(int i=0;i<number_of_nodes;i++){
	// 	printf("----------\n");
	// 	printf("LABEL:: %s\n", nodes[i].label);
	// 	printf("SUBJECT:: %s\n", nodes[i].subject);
	// 	printf("----------\n");
	// }

	// case 1
	// say I just performed /bar; so my root_dirname is /bar
	// but while asking for attribute it was split to / and bar (root_dirname, object_name)
	// so we will perform a quick concatnation and check if it's same as container.root_dirname

	if(strcmp(root_dirname,container.root_dirname)){
		char tmp[10000];
		memset(tmp,0,10000);
		strcat(tmp,root_dirname);
		strcat(tmp,object_name);
		if(strcmp(tmp,container.root_dirname)){
			printf("ROOTDIR NOT SAME! [%s]::[%s]:::: [%s](\n", root_dirname, container.root_dirname, object_name);
			return 0;
		}
		printf("Saved! [%s]::[%s]\n", root_dirname, container.root_dirname);
		// it means that [case 1] is hit
		return 1;
	}

	for(int i=0;i<container.number_of_dirs;i++){
		if(strcmp(object_name,container.dirs[i])==0) return 1;
	}
	for(int i=0;i<container.number_of_files;i++){
		if(strcmp(object_name,container.filenames[i])==0) return 2;
	}
	return 0;
}

// reset and put the new dir as dirname
void push_root_dir(const char* root_dirname){
	// we don;t need to deallocate or do anything fancy. Things are global and static. :)
	container.number_of_files=0;
	container.number_of_dirs=0;
	container.is_valid=1;
	container.root_dirname=strdup(root_dirname);
}

void push_object(const char* objectname, const char* root_dirname, int is_dir){
	assert(strcmp(root_dirname, container.root_dirname)==0);
	if(is_dir){
		container.dirs[container.number_of_dirs++]=strdup(objectname);
	}else{
		container.filenames[container.number_of_files++]=strdup(objectname);
	}
}


#endif
