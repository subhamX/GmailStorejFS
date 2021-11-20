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

// return 1 if dir, 2 if file, 0 if not found
int fetch_object_type(CURL* curl, const char* path, const char* base_url){
  if(!curl) return 1;

	char root_dirname[10000];
	char objectname[10000];

	split_path_to_components(root_dirname, objectname, path);
	printf("Debug: root_dirname: %s\n", root_dirname);
	printf("Debug: objectname: %s\n", objectname);


	if(strcmp(root_dirname, "/")==0){
		char* labels_ptr[MAX_DIRS_IN_A_DIR];
		int number_of_labels=0;
		// look in the folders dir
		fetch_all_labels(curl,labels_ptr,&number_of_labels,base_url);
		for(int i=0;i<number_of_labels;i++){
			if(strcmp(objectname,labels_ptr[i])==0) return 1;
		}
	}

	// look if the email exists
	int msg_id=fetch_msgid_by_subject_and_label(curl, objectname, root_dirname);
	if(msg_id==-1) return 0;
	// it's a file. :)
	return 2;
}


#endif
