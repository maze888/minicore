#ifndef _MCO_ENCODE_H_
#define _MCO_ENCODE_H_

#include "mco.h"

int mco_bin2str(const unsigned char *src, int src_len, char *dst, int dst_len);
int mco_str2bin(const char *src, unsigned char *dst, int *dst_len);

#endif
