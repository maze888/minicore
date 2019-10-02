#ifndef _MCO_UTIL_H_
#define _MCO_UTIL_H_

#include "mco.h"

long mco_get_file_len(const char *path);

void mco_hex_print(FILE *fp, const char *subject, const unsigned char *bin, int len);
void mco_hex_cprint(const char *subject, const unsigned char *bin, int len);

#endif
