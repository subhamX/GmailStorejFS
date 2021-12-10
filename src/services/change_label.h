#ifndef CHANGE_LABEL_H
#define CHANGE_LABEL_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "../utils/definitions.h"
#include "fetch_all_emails_by_label.h"
#include "relabel_an_email.h"
#include "delete_label.h"

/**
 * @brief change the label from one to another
 *
 * @param curl: pointer to the curl handle
 * @param current_label: current label
 * @param new_label: new label
 * @return int: 0 if success, 1 otherwise
 */
int change_label(CURL* curl, const char* current_label, const char* new_label){
	// fetch all mails with specified [current_label]
	char query[10000];
  memset(query, 0, 10000);
  strcat(query, "SEARCH X-GM-LABELS ");
  strcat(query, BASE_MAILBOX_LABEL);
	strcat(query, current_label);

	int indices[MAX_FILES_IN_A_DIR];
	int number_of_indices=0;
	// fetch all messages with the specified current label
	int res=fetch_msgid_by_query(curl, query, indices, &number_of_indices);
	if(res) return 1;

	for(int i=0;i<number_of_indices;i++){
		// relabel all mails
		relabel_an_email(curl, new_label, current_label, indices[i]);
	}

	// delete old label
	delete_label(curl, current_label);

	return 0;
}


#endif
