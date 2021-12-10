#ifndef RELABEL_AN_EMAIL_H
#define RELABEL_AN_EMAIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <regex.h>
#include <assert.h>
#include "../utils/definitions.h"
#include "add_label_to_email.h"
#include "delete_label_from_email.h"


/**
 * @brief method to relabel an email
 *
 * @param curl: pointer to the curl handle
 * @param new_folder_name: new folder/label name
 * @param old_folder_name: old folder/label name
 * @param id: message id
 * @return int: returns 1 if unsuccessful; 0 if successful
 */
int relabel_an_email(CURL* curl, const char* new_folder_name, const char* old_folder_name, int id){
  if(!curl) return 1;

	// delete label from email
	int res=delete_label_from_email(curl, id, old_folder_name);
	if(res){
    printf("Err: Unable to relabel the email; failed: %s\n", curl_easy_strerror(res));
		return 1;
	}

	// add the new label to email
	res=add_label_to_email(curl, id, new_folder_name);
	if(res){
    printf("Err: Unable to relabel the email; failed: %s\n", curl_easy_strerror(res));
		return 1;
	}

	return 0;
}


#endif
