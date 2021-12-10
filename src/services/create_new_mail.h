#ifndef CREATE_NEW_MAIL_H
#define CREATE_NEW_MAIL_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <uuid/uuid.h>
#include "../utils/uuid_ops.h"
#include "add_label_to_email.h"

/**
 * @brief struct definitions for upload_data_node which shall contain the data and the number of bytes sent so far
 *
 */
struct upload_data_node {
  size_t bytes_read;
	char* payload;
};

/**
 * @brief function to allow curl handle get request data in chunks
 */
static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp){
  struct upload_data_node *upload_ctx = (struct upload_data_node *)userp;
  const char *data;
  size_t room = size * nmemb;

  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }

  data = &upload_ctx->payload[upload_ctx->bytes_read];

  if(*data) {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;

    return len;
  }

  return 0;
}

/**
 * @brief Create a new mail and add the specified folder_name aka label
 *
 * @param curl: pointer instance of the CURL handle
 * @param folder_name: folder_name or the label which we shall add to the email
 * @param subject: subject text which we shall put in the email
 * @return int: returns 1 if unsuccessful; 0 if successful
 */
int create_new_mail(CURL* curl, const char* folder_name, const char* subject, const char* content){
  if(!curl) return 1;


	char random_msg_id[UUID_STR_LEN]={0};
	get_uuid(random_msg_id);
	printf("Debug: RANDOM ID: %s\n", random_msg_id);


	long infilesize;
	struct upload_data_node upload_ctx;
	upload_ctx.bytes_read=0;
	upload_ctx.payload=(char*)malloc(sizeof(char)*MAX_LENGTH_OF_EMAIL_IN_STD_FORMAT);
	memset(upload_ctx.payload, 0, MAX_LENGTH_OF_EMAIL_IN_STD_FORMAT);
	strcat(upload_ctx.payload, "To: ");
	strcat(upload_ctx.payload, random_msg_id);
	strcat(upload_ctx.payload, "\r\n");
	// strcat(upload_ctx.payload, "To: hellosubham\r\n");
	strcat(upload_ctx.payload, "Subject: ");
	strcat(upload_ctx.payload, subject);
	/* empty line to divide headers from body, see RFC5322 */
	strcat(upload_ctx.payload, "\r\n");
	if(content) strcat(upload_ctx.payload, content);

	curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
	curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
 	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1000000L);

	infilesize = strlen(upload_ctx.payload);
	curl_easy_setopt(curl, CURLOPT_INFILESIZE, infilesize);

  /* Perform the fetch */
  int res = curl_easy_perform(curl);

	free(upload_ctx.payload);
  if(res != CURLE_OK){
    printf("Err: Unable to create new email; failed: %s\n", curl_easy_strerror(res));
    return 1;
  }

	printf("Debug: PUSH SUCCESSFUL\n");
	// this was necessary
 	curl_easy_setopt(curl, CURLOPT_UPLOAD, NULL);
 	curl_easy_setopt(curl, CURLOPT_READDATA, NULL);
 	curl_easy_setopt(curl, CURLOPT_INFILESIZE, NULL);
 	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

	// fetch the id of the mail
	char query[10000];
	memset(query,0,10000);
	strcat(query, "SEARCH HEADER Subject \"");
	strcat(query, subject);
	strcat(query, "\" TO ");
	strcat(query, random_msg_id);
	int indices[MAX_FILES_IN_A_DIR];
	int number_of_files=0;
	fetch_msgid_by_query(curl, query, indices, &number_of_files);
	if(number_of_files!=1){
		printf("Err: duplicate exist. This shouldn't have happened. :(\n");
		return 1;
	}

	printf("DEBUG: GOT THE ACTUAL INDEX: %d\n", indices[0]);

	// add label to mail
	add_label_to_email(curl, indices[0], folder_name);

	return 0;

}

#endif
