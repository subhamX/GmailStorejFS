#ifndef FETCH_OBJECT_TYPE_H
#define FETCH_OBJECT_TYPE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <regex.h>
#include <assert.h>
#include "../utils/definitions.h"
#include "../utils/string_helpers.h"
#include "fetch_all_labels.h"
#include "fetch_msgid_by_query.h"
#include "../utils/hashmap.h"




// return 1 if dir, 2 if file, 0 if not found
int fetch_object_type_by_dirname_and_objectname(CURL* curl, const char* root_dirname, const char* objectname, const char* base_url){
  if(!curl) return 1;

	char full_object_path[10000];
	memset(full_object_path,0,10000);
	strcat(full_object_path,root_dirname);
	strcat(full_object_path,"/");
	strcat(full_object_path,objectname);
	int object_type=search_hashmap(full_object_path);
	if(object_type!=-1){
		printf("\033[0;32m");
		printf("Cache hit\n");
		printf("\033[0m");
		return object_type;
	}

	object_type=0;
	// look if the email exists
	int msg_id=fetch_msgid_by_subject_and_label(curl, objectname, root_dirname);
	if(msg_id!=-1){
		object_type=2;
	}else if(strcmp(root_dirname, "/")==0){
		// ! DEBUG: returning 1
		char* labels_ptr[MAX_DIRS_IN_A_DIR];
		int number_of_labels=0;
		// look in the folders dir
		fetch_all_labels(curl,labels_ptr,&number_of_labels,base_url);
		for(int i=0;i<number_of_labels;i++){
			if(strcmp(objectname,labels_ptr[i])==0){
				object_type=1;
			}
		}
	}
	push_object(full_object_path, object_type);
	return object_type;
}


// return 1 if dir, 2 if file, 0 if not found
int fetch_object_type(CURL* curl, const char* path, const char* base_url){
  if(!curl) return 1;

	char root_dirname[10000];
	char objectname[10000];

	split_path_to_components(root_dirname, objectname, path);
	printf("Debug: root_dirname: %s\n", root_dirname);
	printf("Debug: objectname: %s\n", objectname);

	return fetch_object_type_by_dirname_and_objectname(curl, root_dirname, objectname, base_url);
}



#endif
