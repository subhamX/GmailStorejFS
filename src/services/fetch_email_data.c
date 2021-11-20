#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <regex.h>
#include <assert.h>


// ! We won't have any hashmap rather will have a function fetch_email_id_by_label_and_subject


int create_new_email_with_label(){
	// push to hash map?
}

// https://curl.se/libcurl/c/imap-store.html
int update_email_with_id(){

}

// TODO:
// what if two has same subject as "hello" (with diff label say)
// we need a way to store the id
// hash map type?
// [label]--[subject] --> id
// so effectively we need to implement [fetch_email_contents_by_id]
char* fetch_email_contents_by_subject(){

}





int remove_email_by_subject(){

	// from subject get

}





static size_t writefunc(void *contents, size_t size, size_t nmemb, void *userp){
  /* ptr - your string variable.
      stream - data chuck you received */

  int actual_length=0;
  // the first line of fetch contains the data length



  // printf("--------START----------\n\n");
  // printf("DEBUG: Got data of length %ld\n", size * nmemb);
  // printf("DEBUG: Actual data of length %ld\n", strlen((char*)contents));
  // printf("DEBUG: Data: %s\n", (char*) contents);
  // printf("--------END----------\n\n");


  size_t realsize = size * nmemb;
  // size_t realsize = strlen((char*)contents);
  struct string *mem = (struct string *)userp;

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




int main(void){
  CURL *curl;
  CURLcode res = CURLE_OK;

  curl = curl_easy_init();
  if(curl) {

    struct string s;
    init_string(&s);

    /* Set username and password */
    // curl_easy_setopt(curl, CURLOPT_USERNAME, "canaryfs@outlook.com");
    // curl_easy_setopt(curl, CURLOPT_PASSWORD, "#489cs303isthecourse142**10$");
    curl_easy_setopt(curl, CURLOPT_USERNAME, "canaryfs7@gmail.com");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "@q$ZYiC&UP@T9vW*w8CTaRL%vBmPUYj");


    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&s);
    curl_easy_setopt(curl, CURLOPT_HEADER, 10000L);


    /* This will fetch message 1 from the user's inbox. Note the use of
    * imaps:// rather than imap:// to request a SSL based connection. */
    // curl_easy_setopt(curl, CURLOPT_URL,"imaps://outlook.office365.com:993/INBOX;UIDVALIDITY=1");
    // curl_easy_setopt(curl, CURLOPT_URL,"imaps://imap.gmail.com:993/%5BGmail%5D%2FAll+Mail.com/");


    // curl_easy_setopt(curl, CURLOPT_URL,"imaps://imap.gmail.com:993/INBOX");
    curl_easy_setopt(curl, CURLOPT_URL,"imaps://imap.gmail.com:993/%5BGmail%5D/Drafts");
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
    // curl_easy_setopt(curl, CURLOPT_LENGTH, 120000000L);
    // char* dataaa;
    // curl_easy_getinfo(curl, CURLINFO_TEXT, dataaa);


    // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "LIST \"\" \"*\"");
    // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "STATUS INBOX (MESSAGES)");
    // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "STATUS [Gmail]/Drafts (MESSAGES)");

    // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "SEARCH NEW");

    // fetch subject
    // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "FETCH 1 BODY.PEEK[HEADER.FIELDS (Subject)]");

    // fetch whole msg
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "FETCH 1 BODY.PEEK[TEXT]");



    // header
    struct curl_slist *headers = NULL;
    curl_slist_append(headers, "Accept: application/xml");
    curl_slist_append(headers, "charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "FETCH 1 (X-GM-MSGID)");


    // Print DEBUG INFO
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 100L);

    /* Perform the fetch */
    res = curl_easy_perform(curl);




    FILE *fptr;
    fptr =fopen("./newprogram.txt","w");

    // int last=0;
    // for(int i=0;i<s.len;i++){
    //   if(s.ptr[i]=='\n'){
    //     printf("Length: %d\n", i-last);
    //     for(int j=last;j<=i;j++){
    //       printf("%c", s.ptr[j]);
    //     }
    //     printf("\n");
    //     last=i;
    //   }
    // }

    char* decoded_txt=decode_quoted_printable(s.ptr);
    fprintf(fptr,"%s",decoded_txt);

    fclose(fptr);

    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));



    /* Always cleanup */
    curl_easy_cleanup(curl);
  }

  return (int)res;
}
