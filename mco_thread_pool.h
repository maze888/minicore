#ifndef _MCO_THREAD_POOL_H_
#define _MCO_THREAD_POOL_H_

#include "mco_queue.h"

typedef enum {
	EMERGENCY_SHUTDOWN = 1, // 작업 스레드가 현재 작업을 진행중이더라도 바로 종료
	IMMEDIATE_SHUTDOWN = 2, // 작업 스레드가 현재 작업을 진행중인것만 완료할때까지 기다려서 종료
	DEFERRED_SHUTDOWN  = 3  // 작업 스레드가 현재 남아있는 작업큐를 모두 가져가서 작업을 완료할때까지 기다려서 종료
} mco_thpool_shutdown_t;

typedef struct {
	void (*work)(void *);
	void *arg;
} mco_thpool_task_t;

typedef struct {
	int thread_count;
	int work_count; 
	int shutdown;

	// work queue
	pthread_mutex_t lock;
	pthread_cond_t notify;
	struct mco_queue *queue;
	
	// thread in pool
	pthread_t *threads;

} mco_thpool_t;

mco_thpool_t * mco_thpool_create(int thread_count, int queue_size);
int mco_thpool_destroy(mco_thpool_t **pool, mco_thpool_shutdown_t shutdown);
int mco_thpool_thread_count(mco_thpool_t *pool);
int mco_thpool_work_count(mco_thpool_t *pool);

int mco_thpool_add(mco_thpool_t *pool, mco_thpool_task_t task);

#endif
