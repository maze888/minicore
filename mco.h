#ifndef _MCO_H_
#define _MCO_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#define safe_free(p) if (p) { free(p); p = NULL; }
#define safe_close(fd) if ( fd >= 0 ) { close(fd); fd = -1; }
#define safe_fclose(fp) if ( fp ) { fclose(fp); fp = NULL; }

extern char mco_last_error[1024];

const char * mco_get_last_error();

void mco_hex_print(unsigned char *p, int p_size);

#endif
