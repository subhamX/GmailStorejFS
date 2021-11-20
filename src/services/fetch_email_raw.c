#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <regex.h>
#include <assert.h>

struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
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
