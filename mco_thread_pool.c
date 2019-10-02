#include "mco_thread_pool.h"

static void * mco_thpool_worker(void *thpool)
{
	mco_thpool_t *pool = (mco_thpool_t *)thpool;
	mco_thpool_task_t task;

	for (;;) {
	}
}

mco_thpool_t * mco_thpool_create(int thread_count, int queue_size)
{
	mco_thpool_t *pool = NULL;

	if ( thread_count <= 0 ) {
		MCO_SET_ERROR("invalid argument. thread_count: %d", thread_count);
		goto out;
	}

	pool = (mco_thpool_t *)calloc(1, sizeof(mco_thpool_t));
	if ( !pool ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}
	
	if ( pthread_mutex_init(&pool->lock, NULL) != 0 ) {
		MCO_SET_ERROR("pthread_mutex_init() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}
	
	if ( pthread_cond_init(&pool->notify, NULL) != 0 ) {
		MCO_SET_ERROR("pthread_cond_init() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}

	if ( !(pool->queue = mco_queue_create(NULL, queue_size)) ) {
		goto out;
	}

	pool->threads = (pthread_t *)calloc(1, thread_count * sizeof(pthread_t));
	if ( !pool->threads ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}

	return pool;

out:
	mco_thpool_destroy(&pool, IMMEDIATE_SHUTDOWN);

	return NULL;
}

int mco_thpool_destroy(mco_thpool_t **pool, mco_thpool_shutdown_t shutdown)
{
	mco_thpool_t *tmp_pool;

	if ( !pool ) {
		MCO_SET_ERROR("invalid argument. **pool: NULL");
		return -1;
	}
	
	if ( !*pool ) {
		MCO_SET_ERROR("invalid argument. *pool: NULL");
		return -1;
	}

	tmp_pool = *pool;

	switch (shutdown) {
		case IMMEDIATE_SHUTDOWN:
			for ( int i = 0; tmp_pool->threads[i] && i < tmp_pool->thread_count; i++ ) {
				if ( pthread_cancel(tmp_pool->threads[i]) != 0 ) {
					MCO_SET_ERROR("pthread_cancel() is failed: %s (errno: %d)", strerror(errno), errno);
					return -1;
				}
			}
			break;
		default: // GRACEFUL_SHUTDOWN
			// TODO: 현재 작업중인 스레드가 작업을 완료할때까지 기다려서 종료 처리하도록
			break;
	}

	mco_queue_free(&tmp_pool->queue);
	pthread_cond_destory(&tmp_pool->notify);
	pthread_mutex_destory(&tmp_pool->lock);

	safe_free(tmp_pool->threads);

	return 0;
}

int mco_thpool_thread_count(mco_thpool_t *pool)
{
	return pool ? pool->thread_count : -1;
}

int mco_thpool_work_count(mco_thpool_t *pool)
{
	return pool ? pool->work_count : -1;
}
