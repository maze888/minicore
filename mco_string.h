#ifndef _MCO_STRING_H_
#define _MCO_STRING_H_

#include "mco.h"

/**
 * @brief   지정한 버퍼에 지정한 문자열을 추가한다.
 *
 * @param   dest    지정한 버퍼
 * @param   src     지정한 문자열
 * @param   dsize   지정한 버퍼의 크기
 *
 * @return  size_t  지정한 버퍼에 저장된 문자열의 길이 
 */
size_t mco_strncat(char *dest, const char *src, size_t dsize);

#endif
