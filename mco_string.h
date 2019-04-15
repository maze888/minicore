#ifndef _MCO_STRING_H_
#define _MCO_STRING_H_

#include "mco.h"

size_t mco_strncat(char *dest, const char *src, size_t dsize);
size_t mco_left_trim(char *buf);
size_t mco_right_trim(char *buf);

#endif
