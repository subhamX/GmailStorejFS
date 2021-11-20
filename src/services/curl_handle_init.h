#ifndef CURL_HANDLE_INIT_H
#define CURL_HANDLE_INIT_H
#include <curl/curl.h>
#include "../utils/definitions.h"


/**
 * @brief function to initialize the curl handle with email, password and URL
 *
 * @param curl: pointer instance of the CURL handle
 * @param email: mailbox email address
 * @param password: mailbox password
 */
void curl_handle_init(CURL **curl, mailbox_config* config){
    // CURLcode res = CURLE_OK;
    (*curl) = curl_easy_init();

    if(!(*curl)) return;
    /* Set username and password */
    curl_easy_setopt(*curl, CURLOPT_USERNAME, config->email);
    curl_easy_setopt(*curl, CURLOPT_PASSWORD, config->password);

		// debug
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 100L);


    // curl_easy_setopt(*curl, CURLOPT_URL,"imaps://imap.gmail.com:993/INBOX");
		// %5BGmail%5D/Drafts
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
void curl_handle_reset(CURL **curl, mailbox_config* config){
		printf("CURL RESET::\n");

    if(!(*curl)) return;

    /* Set username and password */
    curl_easy_setopt(*curl, CURLOPT_USERNAME, config->email);
    curl_easy_setopt(*curl, CURLOPT_PASSWORD, config->password);
		char url[10000];
		memset(url, 0, 10000);
		strcat(url, config->ip_address);
		strcat(url, ":");
		strcat(url, config->port);
		strcat(url, "/%5BGmail%5D/Drafts");
    // curl_easy_setopt(*curl, CURLOPT_URL,"imaps://imap.gmail.com:993/INBOX");
    curl_easy_setopt(*curl, CURLOPT_URL,url);
    curl_easy_setopt(*curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);


}


#endif
