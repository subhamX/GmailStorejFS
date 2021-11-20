#ifndef DELETE_LABEL_FROM_EMAIL_H
#define DELETE_LABEL_FROM_EMAIL_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "../utils/definitions.h"
#include "../utils/string_helpers.h"




// returns 1 if unsuccessful; 0 if successful
int delete_label_from_email(CURL* curl, int id, const char* folder_name){
  if(!curl) return 1;

  char tmp[1000];
  memset(tmp, 0, 1000);

  strcat(tmp, "STORE ");
	char id_str[MAX_LENGTH_OF_LABEL];
	// no memset needed as it will be applied by [int_to_string]
	int_to_string(id_str, MAX_LENGTH_OF_LABEL, id);

	strcat(tmp, id_str);
	strcat(tmp, " -X-GM-LABELS ");
  strcat(tmp, BASE_MAILBOX_LABEL);
  strcat(tmp, folder_name);

	printf("DEBUG: %s\n", tmp);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, tmp);

  /* Perform the fetch */
  int res = curl_easy_perform(curl);
	printf("DONE %s\n", tmp);

	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);

  if(res != CURLE_OK){
    printf("Err: Unable to remove label; failed: %s\n", curl_easy_strerror(res));
    return 1;
  }else{
    printf("debug: Label %s removed successfully from email\n", folder_name);
    return 0;
  }
}

#endif
