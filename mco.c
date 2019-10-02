/**
 * @file    mco.c
 * @brief   libminicore 공통 함수 정의
 * @date    2019.09.20
 * @author  naze@dreamsecurity.com
 *
 */

#include "mco.h"

typedef void *(*memset_t)(void *, int, size_t);

static volatile memset_t memset_func = memset;

/**
 * @brief   Return error that occurred in library
 *
 * @return  Last error in that occrued in library
 *
 */
const char * mco_get_last_error()
{
	return mco_last_error;
}

void mco_cleanse(void *p, size_t len)
{
	memset_func(p, 0x00, len);
}
