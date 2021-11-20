#ifndef CHANGE_LABEL_H
#define CHANGE_LABEL_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "../utils/definitions.h"
#include "fetch_all_emails_by_label.h"
#include "relabel_an_email.h"
#include "delete_label.h"

// by emails we mean email subjects only!
// return 1 if err
int change_label(CURL* curl, const char* current_label, const char* new_label){

	// fetch all mails with specified [current_label]
	char query[10000];
  memset(query, 0, 10000);
  strcat(query, "SEARCH X-GM-LABELS ");
  strcat(query, BASE_MAILBOX_LABEL);
	strcat(query, current_label);


	int indices[MAX_FILES_IN_A_DIR];
	int number_of_indices=0;
	int res=fetch_msgid_by_query(curl, query, indices, &number_of_indices);
	if(res) return 1;

	for(int i=0;i<number_of_indices;i++){
		relabel_an_email(curl, new_label, current_label, indices[i]);
	}

	// delete old label
	delete_label(curl, current_label);

	return 0;
}


#endif
