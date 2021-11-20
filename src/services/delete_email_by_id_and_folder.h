#ifndef DELETE_EMAIL_BY_ID_AND_FOLDER_H
#define DELETE_EMAIL_BY_ID_AND_FOLDER_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "../utils/string_helpers.h"
#include "../utils/definitions.h"
#include "delete_label_from_email.h"



/**
 * @brief function to remove email from the mailbox
 * Note: Although id itself is enough to establish uniqueness, we need `current_folder` to remove the current label, without it EXPUNGE are ignored
 * @param curl: pointer instance of the CURL handle
 * @param id: unique id of the email
 * @param current_folder: current_folder_name or the label which we shall remove from the email before deleting
 * @return int: returns 1 if unsuccessful; 0 if successful
 */
int delete_email_by_id_and_folder(CURL* curl, int id, char* current_folder){
  if(!curl) return 1;

	char id_str[1000];
  memset(id_str, 0, 1000);
	int_to_string(id_str, 1000, id);

	printf("Yayayayaya\n");

  int res = delete_label_from_email(curl, id, current_folder);
  if(res != CURLE_OK){
		printf("Err: Remove label unsuccessful\n");
		return 1;
	}else{
		printf("Debug: Remove label successful\n");
	}

	char tmp[10000];
  memset(tmp, 0, 10000);
  strcat(tmp, "STORE ");
  strcat(tmp, id_str);
  strcat(tmp, " +Flags \\Deleted");

	printf("Query: %s\n", tmp);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, tmp);

  /* Perform the fetch */
  res = curl_easy_perform(curl);

	// reset
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);

  if(res != CURLE_OK){
    printf("Err: Unable to add deleted label; failed: %s\n", curl_easy_strerror(res));
    return 1;
  }else{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXPUNGE");

	  res = curl_easy_perform(curl);

		// reset
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
   /* Check for errors */
		if(res != CURLE_OK){
			printf("Err: Unable to EXPUNGE; failed: %s\n", curl_easy_strerror(res));
	    return 1;
		}else{
			printf("Err: Msg deleted successfully\n");
		}
    return 0;
  }
}



#endif
