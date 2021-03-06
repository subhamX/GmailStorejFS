#ifndef CURL_HANDLE_INIT_H
#define CURL_HANDLE_INIT_H
#include <curl/curl.h>
#include "../utils/definitions.h"


 mailbox_config* global_config_ptr=NULL;

/**
 * @brief function to initialize the curl handle with email, password and URL
 *
 * @param curl: pointer instance of the CURL handle
 * @param email: mailbox email address
 * @param password: mailbox password
 */
void curl_handle_init(CURL **curl, mailbox_config* config){
 	global_config_ptr=config;
	// CURLcode res = CURLE_OK;
	(*curl) = curl_easy_init();

	if(!(*curl)) return;
	/* Set username and password */
	curl_easy_setopt(*curl, CURLOPT_USERNAME, config->email);
	curl_easy_setopt(*curl, CURLOPT_PASSWORD, config->password);

	// debug
	// curl_easy_setopt(curl, CURLOPT_VERBOSE, 100L);


	char url[10000];
	memset(url, 0, 10000);
	strcat(url, config->ip_address);
	strcat(url, ":");
	strcat(url, config->port);
	strcat(url, "/%5BGmail%5D/Drafts");
	// "imaps://imap.gmail.com:993/"
	curl_easy_setopt(*curl, CURLOPT_URL,url);
	curl_easy_setopt(*curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

}

/**
 * @brief function to re-initialize the curl handle and add email, password and URL
 *
 * @param curl: pointer instance of the CURL handle
 * @param email: mailbox email address
 * @param password: mailbox password
 */
void custom_curl_handle_reset(CURL **curl){
	printf("CURL RESET:: %d\n", (*curl==NULL));

	curl_easy_reset(*curl);
	printf("CURL RESET:: %d\n", (*curl==NULL));

	if(!(*curl)) return;

	/* Set username and password */
	curl_easy_setopt(*curl, CURLOPT_USERNAME, global_config_ptr->email);
	curl_easy_setopt(*curl, CURLOPT_PASSWORD, global_config_ptr->password);
	char url[10000];
	memset(url, 0, 10000);
	strcat(url, global_config_ptr->ip_address);
	strcat(url, ":");
	strcat(url, global_config_ptr->port);
	strcat(url, "/%5BGmail%5D/Drafts");
	curl_easy_setopt(*curl, CURLOPT_URL,url);
	curl_easy_setopt(*curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);


}


#endif
