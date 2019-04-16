#include "mco_encode.h"
#include "mco_hidden.h"

int mco_bin2str(unsigned char *src, int src_len, char *dst, int dst_len)
{
  if ( dst_len <= src_len * 2 ) {
		MCO_SET_ERROR("not enough buffer");
    return -1;
  }

  memset(dst, 0x00, dst_len);

  unsigned char uTable[256] = {0};

  uTable[0x00] = '0'; uTable[0x01] = '1'; uTable[0x02] = '2'; uTable[0x03] = '3';
  uTable[0x04] = '4'; uTable[0x05] = '5'; uTable[0x06] = '6'; uTable[0x07] = '7';
  uTable[0x08] = '8'; uTable[0x09] = '9'; uTable[0x0a] = 'a'; uTable[0x0b] = 'b';
  uTable[0x0c] = 'c'; uTable[0x0d] = 'd'; uTable[0x0e] = 'e'; uTable[0x0f] = 'f';

  uTable[0x10] = '1'; uTable[0x20] = '2'; uTable[0x30] = '3'; uTable[0x40] = '4';
  uTable[0x50] = '5'; uTable[0x60] = '6'; uTable[0x70] = '7'; uTable[0x80] = '8';
  uTable[0x90] = '9'; uTable[0xa0] = 'a'; uTable[0xb0] = 'b'; uTable[0xc0] = 'c';
  uTable[0xd0] = 'd'; uTable[0xe0] = 'e'; uTable[0xf0] = 'f'; 

  for (int i = 0, j = 0; i < src_len; i++) {
    dst[j++] = uTable[src[i] & 0xf0];
    dst[j++] = uTable[src[i] & 0x0f];
  }

  return 0;
}

int mco_str2bin(char *src, unsigned char *dst, int *dst_len)
{
  int dst_size = strlen(src) / 2;

  if ( *dst_len <= dst_size ) {
		MCO_SET_ERROR("not enough buffer");
    return -1;
  }

  memset(dst, 0x00, *dst_len);

  unsigned char uTable0[512], uTable1[512];
  memset(uTable0, 0x00, sizeof(uTable0));
  memset(uTable1, 0x00, sizeof(uTable1));

  uTable1['1'] = 0x01; uTable1['2'] = 0x02; uTable1['3'] = 0x03; uTable1['4'] = 0x04;
  uTable1['5'] = 0x05; uTable1['6'] = 0x06; uTable1['7'] = 0x07; uTable1['8'] = 0x08;
  uTable1['9'] = 0x09; uTable1['A'] = 0x0a; uTable1['B'] = 0x0b; uTable1['C'] = 0x0c;
  uTable1['D'] = 0x0d; uTable1['E'] = 0x0e; uTable1['F'] = 0x0f; uTable1['a'] = 0x0a;
  uTable1['b'] = 0x0b; uTable1['c'] = 0x0c; uTable1['d'] = 0x0d; uTable1['e'] = 0x0e;
  uTable1['f'] = 0x0f;
  
  uTable0['1'] = 0x10; uTable0['2'] = 0x20; uTable0['3'] = 0x30; uTable0['4'] = 0x40;
  uTable0['5'] = 0x50; uTable0['6'] = 0x60; uTable0['7'] = 0x70; uTable0['8'] = 0x80;
  uTable0['9'] = 0x90; uTable0['A'] = 0xa0; uTable0['B'] = 0xb0; uTable0['C'] = 0xc0;
  uTable0['D'] = 0xd0; uTable0['E'] = 0xe0; uTable0['F'] = 0xf0; uTable0['a'] = 0xa0;
  uTable0['b'] = 0xb0; uTable0['c'] = 0xc0; uTable0['d'] = 0xd0; uTable0['e'] = 0xe0;
  uTable0['f'] = 0xf0;
  
  for (int i = 0; i < dst_size; i++, src += 2) {
    dst[i] = uTable0[(unsigned int)*src] + uTable1[(unsigned int)(*(src + 1))];
  }

  *dst_len = dst_size;

  return 0;
}
