#ifndef CANARY_DECODE_H
#define CANARY_DECODE_H
#include "custom_string.h"

/* hexval -- return value of a hexadecimal digit (0-9A-F) */
static int hexval(int c){
  if ('0' <= c && c <= '9') return c - '0';
  assert('A' <= c && c <= 'F');
  return 10 + c - 'A';
}

/* decode_char -- return the character corresponding to "=XX" */
static int decode_char(const char *s){
  assert(s);
  assert(*s == '=');
  return 16 * hexval(*(s+1)) + hexval(*(s+2));
}

/* decode_quoted_printable -- decode and print a quoted-printable string */
static char* decode_quoted_printable(custom_string *body){
  assert(body->ptr);
  int sz=100000;
  char* response=(char*)malloc(sizeof(char)*sz);
  memset(response, 0, sizeof(char)*sz);
  int k=0;
	// printf("Len: %d [%c]\n", body->len, body->ptr[body->len-3]);
	// return;
	for(int i=0;i<body->len;){
    if (body->ptr[i] != '=') response[k++]=(body->ptr[i++]);
    else if ((body->ptr[i+1]) == '\r' && (body->ptr[i+2]) == '\n') i += 3;
    else if ((body->ptr[i+1]) == '\n') i += 2;
    else if (!strchr("0123456789ABCDEF", (body->ptr[i+1]))) response[k++]=(body->ptr[i++]);
    else if (!strchr("0123456789ABCDEF", (body->ptr[i+2]))) response[k++]=(body->ptr[i++]);
    else {response[k++]=decode_char(body->ptr+i); i += 3;}
  }
	// free old memory
	free(body->ptr);
	body->ptr=response;
	body->len=k;
  return response;
}



#endif
