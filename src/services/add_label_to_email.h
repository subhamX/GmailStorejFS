#ifndef ADD_LABEL_TO_MAIL_H
#define ADD_LABEL_TO_MAIL_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "../utils/definitions.h"
#include "../utils/string_helpers.h"



/**
 * @brief function to add label to an existing email
 *
 * @param curl: pointer instance of the CURL handle
 * @param id: unique id of the email
 * @param folder_name: folder_name or the label which we shall add to the email
 * @return int: returns 1 if unsuccessful; 0 if successful
 */
int add_label_to_email(CURL* curl, int id, char* folder_name){
  if(!curl) return 1;

	char id_str[MAX_LENGTH_OF_LABEL];
	// no memset needed as it will be applied by [int_to_string]
	int_to_string(id_str, MAX_LENGTH_OF_LABEL, id);

  char tmp[1000];
  memset(tmp, 0, 1000);
  strcat(tmp, "STORE ");
	strcat(tmp, id_str);
	strcat(tmp, " +X-GM-LABELS ");
  strcat(tmp, BASE_MAILBOX_LABEL);
  strcat(tmp, folder_name);

	// printf("DEBUG: %s\n", tmp);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, tmp);

  /* Perform the fetch */
  int res = curl_easy_perform(curl);
  if(res != CURLE_OK){
    printf("Err: Unable to add label to email; handle_msg: %s\n", curl_easy_strerror(res));
    return 1;
  }else{
    printf("debug: Label %s added successfully to email\n", folder_name);
    return 0;
  }
}

#endif
