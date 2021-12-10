#ifndef EMAILS_BY_LABEL_H
#define EMAILS_BY_LABEL_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "../utils/definitions.h"
#include "fetch_msgid_by_query.h"
#include "fetch_email_subject_by_id.h"

/**
 * @brief function to fetch all emails with the specified label
 *
 * @param curl: pointer to the curl handle
 * @param current_label: label name
 * @param email_subjects: pointer to char* to get the email subject
 * @param size: pointer to integer to get the size of the email subjects
 * @return int: 0 if success, 1 otherwise
 */

int fetch_all_emails_by_label(CURL* curl, const char* current_label, char** email_subjects, int* size){
	// build query to fetch all the msgid having the specified label
  char query[1000];
  memset(query, 0, 1000);
  strcat(query, "SEARCH X-GM-LABELS ");
  strcat(query, BASE_MAILBOX_LABEL);
	strcat(query, current_label);

	int indices[MAX_FILES_IN_A_DIR];
	int number_of_indices=0;

	// fetch all the msg id's
	int res=fetch_msgid_by_query(curl, query, indices, &number_of_indices);
	if(res){
		fprintf(stderr, "Unable to execute query %s\n", query);
		return 1;
	}

	// fetch the email's subject
	for(int i=0;i<number_of_indices;i++){
		char* subject=fetch_email_subject_by_id(curl, indices[i]);
		(email_subjects)[(*size)++]=subject;
	}

	return 0;
}


#endif
