#ifndef _MCO_H_
#define _MCO_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>

#define safe_free(p) if (p) { free(p); p = NULL; }
#define safe_close(fd) if ( fd >= 0 ) { close(fd); fd = -1; }
#define safe_fclose(fp) if ( fp ) { fclose(fp); fp = NULL; }
#define safe_closedir(dirp) if ( dirp ) { closedir(dirp); dirp = NULL; }

#define NULLSTR(p) p ? "valid" : "null"

// 자료구조 공통적으로 사용될 callback free function type
typedef void (*mco_data_struct_node_free)(void *node, size_t len);

extern char mco_last_error[1024];

const char * mco_get_last_error();

void mco_cleanse(void *p, size_t len);

#endif
