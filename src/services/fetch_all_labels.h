#ifndef FETCH_ALL_LABEL_H
#define FETCH_ALL_LABEL_H
#include "../utils/write_callback.h"
#include "../utils/definitions.h"
#include "../utils/custom_string.h"
#include <assert.h>

/**
 * @brief method to parse the response text and get the canonical label (i.e. after removing the current_full_path)
 *
 * @param str: response text from the imap server
 * @param current_full_path: current path
 * @return char*: label
 */
char* fetch_label(char* str, char* current_full_path){
	char* label=(char*)malloc(sizeof(char)*MAX_LENGTH_OF_LABEL);
	memset(label, 0, MAX_LENGTH_OF_LABEL);

	// printf("MY: %s\n", str);
	// printf("MY: %s\n", current_full_path);

	int i=1;
	while(str[i]!='\0' && !(str[i-1]=='"' && str[i]=='/' && str[i+1]=='"')) i++;

	if(str[i+2]!=' ') return NULL;
	if(str[i+3]!='"') return NULL;
	i+=4;
	int j=0;
	while(current_full_path[j]!='\0' && str[i]==current_full_path[j]){
		i++;
		j++;
	}
	if(current_full_path[j]!='\0') return NULL;

	// writing to label
	j=0;
	while(str[i]!='"'){
		label[j++]=str[i++];
	}

	return label;
}

/**
 * @brief method to parse the given response text and get all the labels
 *
 * @param s: response text
 * @param labels_ptr: pointer to char* to store the labels
 * @param size: pointer to int to store the size of labels_ptr
 * @param current_full_path: current path (token that should be subtracted from the text to extract the label)
 */
void parse_labels_response(custom_string* s, char** labels_ptr, int* size, char* current_full_path){
	char tmp[10000];
	memset(tmp,0,10000);

	int indx=0;
	int first=1;
	for(int i=0;i<s->len;i++){
		if(first){
			first=(s->ptr[i]!='\n');
		}else if(s->ptr[i]=='\n'){
			// process
			char* label=fetch_label(tmp, current_full_path);
			if(label) labels_ptr[(*size)++]=label;
			// reset
			indx=0;
			memset(tmp,0,10000);
		}else{
			tmp[indx++]=s->ptr[i];
		}
	}
}


/**
 * @brief method to fetch all labels
 *
 * > Note: we shall fetch labels of only root label only!
 * @param curl: pointer to curl handle
 * @param labels_ptr: pointer to char* to store the labels
 * @param size: pointer to int to store the size of labels_ptr
 * @param base_full_url: original base url path of the IMAP server
 * @return int: 0 if ok; 1 if error
 */
int fetch_all_labels(CURL* curl, char** labels_ptr, int* size, const char* base_full_url){

  char tmp[1000];
  memset(tmp, 0, 1000);
  strcpy(tmp, base_full_url);

	curl_easy_setopt(curl, CURLOPT_URL, tmp);
	printf("Current: %s\n", tmp);

	custom_string s;
	init_custom_string(&s);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&s);

	int res = curl_easy_perform(curl);

	// reseting to old value
	curl_easy_setopt(curl, CURLOPT_URL, base_full_url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

	/* Check for errors */
	if(res != CURLE_OK){
		printf("Err: Unable to fetch labels; msg:  %s\n", curl_easy_strerror(res));
		return 1;
	}else{
		printf("debug: labels fetch successfully\n");
	}

	char current_full_path[1000];
	memset(current_full_path, 0, 1000);
	strcat(current_full_path, "[Gmail]/Drafts/");
	parse_labels_response(&s, labels_ptr, size, current_full_path);

	return 0;
}


#endif
