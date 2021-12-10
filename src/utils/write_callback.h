#ifndef WRITE_CALLBACK_H
#define WRITE_CALLBACK_H

#include "definitions.h"



/**
 * @brief method to get the specified size of content and push it in the custom_string container
 */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp){
  // contents is the data we just received
  // userp is our variable
  size_t realsize = size * nmemb;
  struct custom_string *mem = (struct custom_string *)userp;

  char *ptr = realloc(mem->ptr, mem->len + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->ptr = ptr;
  memcpy(&(mem->ptr[mem->len]), contents, realsize);
  mem->len += realsize;
  mem->ptr[mem->len] = 0;

  return size * nmemb;
}

/**
 * @brief function to do nothing and act as a dummy callback function
 */
size_t dummy_write_callback(void *buffer, size_t size, size_t nmemb, void *userp){
   return size * nmemb;
}


#endif
