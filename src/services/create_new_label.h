#ifndef CREATE_NEW_LABEL_H
#define CREATE_NEW_LABEL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <regex.h>
#include <assert.h>
#include "../utils/definitions.h"


/**
 * @brief Create a new label aka folder in the mailbox account
 *
 * @param curl: pointer instance of the CURL handle
 * @param new_folder_name: folder_name or the label which we shall create
 * @return int: returns 1 if unsuccessful; 0 if successful
 */
int create_new_label(CURL* curl, char* new_folder_name){
  if(!curl) return 1;

  char tmp[1000];
  memset(tmp, 0, 1000);

  /* Set the CREATE command specifying the new folder name */
  strcat(tmp, "CREATE ");
  strcat(tmp, BASE_MAILBOX_LABEL);
  strcat(tmp, new_folder_name);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, tmp);

  /* Perform the fetch */
  int res = curl_easy_perform(curl);
  if(res != CURLE_OK){
    printf("Err: Unable to create new folder; msg: %s\n", curl_easy_strerror(res));
    return 1;
  }else{
    printf("Debug: Folder %s created successfully! CMD: %s\n", new_folder_name, tmp);
    return 0;
  }
}


#endif
