#ifndef UUID_OPS_H
#define UUID_OPS_H
#include <stdlib.h>
#include <stdio.h>
#include <uuid/uuid.h>


/**
 * @brief Get the uuid object
 */
void get_uuid(char out[UUID_STR_LEN]) {
  uuid_t b;
  uuid_generate(b);
  uuid_unparse_lower(b, out);
}

#endif
