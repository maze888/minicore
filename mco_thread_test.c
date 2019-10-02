/**
 * @file    mco_thread_test.c
 * @brief   스레드 안정성 테스트 지원을 위한 함수 정의
 * @date    2019.09.20
 * @author  naze@dreamsecurity.com
 *
 */

#include "mco_thread_test.h"
#include "mco_inter.h"

struct mco_thread_test {
	int loop_count;

	pthread_barrier_t barrier;

	mco_thread_test_cb test_cb;
	void *test_cb_arg;

	int error; // error happen in thread worker..?
};

static void * test_worker(void *arg)
{
	int rv;
	struct mco_thread_test *tt_arg = (struct mco_thread_test *)arg;

	rv = pthread_barrier_wait(&tt_arg->barrier);
	switch (rv) {
		case 0:
		case PTHREAD_BARRIER_SERIAL_THREAD:
			break;
		default:
			MCO_SET_ERROR("pthread_barrier_wait() is failed: %s (errno: %d)", strerror(errno), errno);
			goto out;
	} 

	for ( int i = 0; i < tt_arg->loop_count; i++ ) {
		tt_arg->test_cb(tt_arg->test_cb_arg);
	}

	return NULL;

out:
	tt_arg->error = 1;

	return NULL;
}

int mco_thread_test(int thread_count, int loop_count, mco_thread_test_cb test_cb, void *test_cb_arg)
{
	struct mco_thread_test tt_arg;

	if ( thread_count > 1024 ) {
		MCO_SET_ERROR("too much thread count: %u (max is 1024)", thread_count);
		return -1;
	}

	mco_cleanse(&tt_arg, sizeof(tt_arg));
	
	tt_arg.loop_count = loop_count;
	tt_arg.test_cb = test_cb;
	tt_arg.test_cb_arg = test_cb_arg;
	
	if ( pthread_barrier_init(&tt_arg.barrier, NULL, thread_count) != 0 ) {
		MCO_SET_ERROR("pthread_barrier_init() is failed: %s (errno: %d)", strerror(errno), errno);
		return -1;
	}
	
	pthread_t tid[thread_count];
	for ( int i = 0; i < thread_count; i++ ) {
		if ( pthread_create(&tid[i], NULL, test_worker, &tt_arg) != 0 ) {
			MCO_SET_ERROR("pthread_create() is failed: %s (errno: %d)", strerror(errno), errno);
			return -1;
		}
	}

	for ( int i = 0; i < thread_count; i++ ) {
		if ( pthread_join(tid[i], NULL) != 0 ) {
			MCO_SET_ERROR("pthread_join() is failed: %s (errno: %d)", strerror(errno), errno);
			return -1;
		}

		if ( tt_arg.error ) return -1;
	}

	if ( pthread_barrier_destroy(&tt_arg.barrier) != 0 ) {
		MCO_SET_ERROR("pthread_barrier_destroy() is failed: %s (errno: %d)", strerror(errno), errno);
		return -1;
	}

	return 0;
}
