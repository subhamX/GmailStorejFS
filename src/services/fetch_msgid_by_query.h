#ifndef FETCH_MSGID_BY_QUERY_H
#define FETCH_MSGID_BY_QUERY_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <assert.h>
#include "../utils/definitions.h"
#include "../utils/string_helpers.h"
#include "curl_handle_init.h"



void parse_email_indices(custom_string* s, int* indices, int* len){
	int i=0;
	char* tmp="* SEARCH";
	int tmp_len=strlen(tmp);
	for(;i<tmp_len;i++){
		assert(s->ptr[i]==tmp[i]);
	}
	if(s->ptr[i]!=' ') return; // no values
	i++;
	int val=0;
	while(i<s->len){
		if(s->ptr[i]==' ' || s->ptr[i]=='\r'){
			indices[(*len)++]=(val);
			val=0;
		}else{
			val*=10;
			val+=(s->ptr[i]-'0');
		}
		i++;
	}
}


// return non zero if error;
int fetch_msgid_by_query(CURL* curl, char* query, int* indices, int* len){
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, query);

	custom_string s;
	init_custom_string(&s);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&s);

  /* Perform the fetch */

	// reset
  int res = curl_easy_perform(curl);
	custom_curl_handle_reset(&curl);

  if(res != CURLE_OK){
    printf("Err: Unable to get the msgid; errmsg: %s\n", curl_easy_strerror(res));
    return 1;
  }

	parse_email_indices(&s, indices, len);
	free(s.ptr);
	return 0;
}


// will be unique; so we shall return the id
// if err then -1
int fetch_msgid_by_subject_and_label(CURL* curl, const char* subject, const char* label){
	// fetch the id of the mail
	char query[10000];
	memset(query,0,10000);
	strcat(query, "SEARCH HEADER Subject \"");
	strcat(query, subject);
	strcat(query, "\" X-GM-LABELS \"");
  strcat(query, BASE_MAILBOX_LABEL);
	strcat(query, label);
	strcat(query, "\"");

	int indices[MAX_FILES_IN_A_DIR];
	int number_of_files=0;
	int res=fetch_msgid_by_query(curl, query, indices, &number_of_files);
	if(res || number_of_files!=1){
		printf("Err: duplicate exist or something went wrong. This shouldn't have happened. :(\n");
		return -1;
	}

	return indices[0];
}

#endif
