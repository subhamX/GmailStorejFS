#ifndef DELETE_LABEL_H
#define DELETE_LABEL_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>


/**
 * @brief function to delete an entrire label
 *
 * > Note that the mails won't be deleted, rather they will only lose the label. So it will give a sense of recursive deletion.
 * > Note that we are not removing h/a, h/a/b if we are asked to remove label 'h'. this is because the assignment assumes that the max depth of the folders will be "1"
 * @param curl: pointer to the curl handle
 * @param folder_name: folder name to be deleted
 * @return int: 0 if success, 1 otherwise
 */
int delete_label(CURL* curl, const char* folder_name){
  if(!curl) return 1;


  char tmp[1000];
  memset(tmp, 0, 1000);

  strcat(tmp, "DELETE ");
  strcat(tmp, BASE_MAILBOX_LABEL);
  strcat(tmp, folder_name);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, tmp);

  /* Perform the fetch */
  int res = curl_easy_perform(curl);
	// reset
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);

  if(res != CURLE_OK){
    printf("Err: Unable to delete label; failed: %s\n", curl_easy_strerror(res));
    return 1;
  }else{
    printf("Debug: Label %s%s deleted successfully\n", BASE_MAILBOX_LABEL, folder_name);
    return 0;
  }
}

#endif
