#ifndef _MCO_SYSTEM_H_
#define _MCO_SYSTEM_H_

#include "mco.h"

struct mco_process_status {
	int alive;
	char name[256];
	uid_t uid;        /* process owner's uid */
	pid_t pid;
	char st_time[64]; /* 프로세스가 시작된 시간(MM-DD hh:mm:ss) */
};   

int mco_getUID(int pid);
time_t mco_get_system_boot_time();
int mco_get_process_status(const char *pname, struct mco_process_status *pstat);
int mco_get_processes_status(const char *pname, struct mco_process_status *pstat, int *size);

int mco_kill_process(pid_t pid);
int mco_kill_process_name(const char *pname);
int mco_killall_process_name(const char *pname);
int mco_killall_owner_process_name(uid_t uid, const char *pname);

int mco_copy_file(const char *spath, const char *dpath, mode_t mode);

#endif
