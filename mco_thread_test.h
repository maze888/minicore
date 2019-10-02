#ifndef _MCO_THREAD_TEST_TOOL_H_
#define _MCO_THREAD_TEST_TOOL_H_

#include "mco.h"

typedef void (*mco_thread_test_cb)(void *arg);

/* 인자로 넘긴 스레드 개수와 루프만큼, 지정된 함수를 돌려주는 함수 
	 테스트 용도로 사용된다.
   0: running success, others: running fail */
int mco_thread_test(int thread_count, int loop_count, mco_thread_test_cb test_cb, void *test_cb_arg);

#endif
