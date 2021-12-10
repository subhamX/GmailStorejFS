#ifndef CUSTOM_STRING_H
#define CUSTOM_STRING_H
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief function to initialize the custom string with NULL values
 * @param s: pointer to the struct of custom_type
 */
void init_custom_string(struct custom_string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}


#endif
