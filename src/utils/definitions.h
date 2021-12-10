#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <curl/curl.h>

#define MAX_DIR_DEPTH 100000
#define MAX_DIRS_IN_A_DIR 100000
#define MAX_FILES_IN_A_DIR 100000
#define MAX_LENGTH_OF_LABEL 100000
#define MAX_LENGTH_OF_FILE_NAME 100000
#define MAX_LENGTH_OF_EMAIL_IN_STD_FORMAT 1000000

#define BASE_MAILBOX_LABEL "[Gmail]/Drafts"
#define DELETED_MAILBOX_LABEL "DELETED"



typedef struct custom_string {
  char *ptr;
  size_t len;
} custom_string;



typedef struct mailbox_config {
	char *email;
	char *password;
	char *ip_address;
	char *port;
} mailbox_config;


typedef struct private_data_node{
	mailbox_config* config;
	CURL* curl;
	char* base_full_url;
	int level; // at max we shall allow only 1 level
} private_data_node;



#endif
