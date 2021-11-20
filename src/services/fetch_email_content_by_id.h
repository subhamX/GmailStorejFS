#ifndef FETCH_EMAIL_CONTENT_H
#define FETCH_EMAIL_CONTENT_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <assert.h>
#include "../utils/definitions.h"
#include "../utils/string_helpers.h"
#include "../utils/decode.h"



char* get_content_from_response_headers(custom_string* s){

	decode_quoted_printable(s);

	printf("RAW CONTENT: %s\n", s->ptr);

	char* sep="FETCH (BODY[TEXT] {";
	int sep_len=strlen(sep);

	int i=0;
	while(i<s->len){
		int j=0;
		while(j<sep_len){
			if(s->ptr[i+j]!=sep[j]) break;
			j++;
		}
		if(j==sep_len){
			i+=j;
			break;
		}
		i++;
	}
	// ---}\n [actual content]
	while(i<s->len && s->ptr[i]!='\n') i++;

	i++;
	// it's body now.

	char* body=(char*)malloc(sizeof(char)*10000);
	memset(body,0,10000);
	int k=0;
	while(s->ptr[i]!='\0'){
		// if(s->ptr[i]=='\r' && s->ptr[i+1]=='\n'){
		// 	i+=2;
		// 	continue;
		// }

		// printf("HIT [%c] [%c] [%d] [%d]\n", s->ptr[i], s->ptr[i+1], s->ptr[i]=='\r', s->ptr[i+1]=='\r');

		if(s->ptr[i]==')' && s->ptr[i+1]=='\r' && s->ptr[i+2]=='\n' && s->ptr[i+3]=='A' && s->ptr[i+4]=='0' && s->ptr[i+5]=='0'){
			break;
		}


		// if(s->ptr[i]!='\r' && s->ptr[i]!='\n'){
			body[k++]=s->ptr[i];
		// }
		// printf("My: [%s]]]\n\n\n", subject);
		i++;
	}
	// printf("BREAK::: %d", k);
	// for(int i=0;i<k;i++){
	// 	printf("%c", subject[i]);
	// }
	// printf("%s\n\n", body);

	return body;

}



// return NULL if error
char* fetch_email_content_by_id(CURL* curl, int id){

	// fetch their subject

	char id_str[MAX_LENGTH_OF_LABEL];
	int_to_string(id_str, MAX_LENGTH_OF_LABEL, id);

	char tmp[10000];
	memset(tmp,0,10000);
	strcat(tmp, "FETCH ");
	strcat(tmp, id_str);
	strcat(tmp, " BODY.PEEK[TEXT]");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, tmp);

	custom_string s;
	init_custom_string(&s);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&s);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dummy_write_callback);

  /* Perform the fetch */
  int res = curl_easy_perform(curl);
	// reset
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

  if(res != CURLE_OK){
    printf("Err: Unable to fetch the email subject; msg: %s\n", curl_easy_strerror(res));
    return NULL;
  }

	// parse
	return get_content_from_response_headers(&s);
}


#endif
