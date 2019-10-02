/**
 * @file    mco_system.c
 * @brief   프로세스 killall 등의 시스템 처리 관련 함수 정의
 * @date    2019.09.20
 * @author  naze@dreamsecurity.com
 *
 */

#include "mco_system.h"
#include "mco_util.h"
#include "mco_inter.h"

#define KILLALL_MAX 1024

static int get_proc_data(const char *path, char *data, size_t len)
{
	FILE *fp;
	
	fp = fopen(path, "rt");
	if ( !fp ) {
		MCO_SET_ERROR("fopen() is failed: %s (errmsg: %s, errno: %d)\n", path, strerror(errno), errno);
		goto out;
	}

	if ( !fgets(data, len, fp) ) {
		MCO_SET_ERROR("fgets() is failed: %s (errmsg: %s, errno: %d)\n", path, strerror(errno), errno);
		goto out;
	}
	
	fclose(fp);

	return 0;

out:
	safe_fclose(fp);

	return -1;
}

/* 시스템 시작 시간 + 시스템 시작 이후 프로세스가 구동된 시간 == 프로세스가 시작된 시간 */
static int get_process_start_time(const char *start_t, struct tm *tm)
{
	time_t t, btime;
		
	btime = mco_get_system_boot_time();
	if ( btime <= 0 ) {
		return -1;
	}

	t = btime + (atoll(start_t) / sysconf(_SC_CLK_TCK));

	localtime_r(&t, tm);

	return 0;
}

int mco_getUID(int pid)
{
	FILE *fp = NULL;
	char buf[256], token[3][128];
	char fname[256] = {0};

	snprintf(fname, sizeof(fname), "/proc/%d/status", pid);

	fp = fopen(fname, "r");
	if ( !fp ) {
		MCO_SET_ERROR("fopen() is failed: %s (errmsg: %s, errno: %d)\n", fname, strerror(errno), errno);
		return -1;
	}

	while ( fgets (buf, sizeof(buf),fp) != NULL ) {
		sscanf(buf, "%s %s", token[0], token[1]);

		if ( !strcasecmp(token[0], "Uid:" ) ) {
			fclose (fp);
			return atoi(token[1]);
		}
	}

	MCO_SET_ERROR("'Uid' not found: %s", fname);

	safe_fclose(fp);

	return -1;
}

time_t mco_get_system_boot_time()
{
	FILE *fp = NULL;
	char buf[1024] = {0};

	fp = fopen("/proc/stat", "r");
	if ( !fp ) {
		MCO_SET_ERROR("fopen() is failed: /proc/stat (errmsg: %s errno: %d)\n", strerror(errno), errno);
		return -1;
	}

	while ( fgets(buf, sizeof(buf) - 1, fp) ) {
		int i = 0;
		char *token, *save_token;

		for ( i = 0, token = strtok_r(buf, " ", &save_token); token; i++, token = strtok_r(NULL, " ", &save_token) ) {
			if ( i == 0 && strcmp(token, "btime") == 0 ) {
				fclose(fp);
				return atoll(save_token);
			}
		}

		mco_cleanse(buf, sizeof(buf));
	}

	fclose(fp);

	MCO_SET_ERROR("'btime' not found: /proc/stat");

	return 0;
}

int mco_get_process_status(const char *pname, struct mco_process_status *pstat)
{
	DIR *dirp = NULL;
	char tmp[32][64];

	mco_cleanse(pstat, sizeof(struct mco_process_status));

	dirp = opendir("/proc");
	if ( !dirp ) {
		MCO_SET_ERROR("opendir() is failed: /proc (errmsg: %s errno: %d)\n", strerror(errno), errno);
		return -1;
	}

	for ( struct dirent *dentry = readdir(dirp); dentry; dentry = readdir(dirp) ) {
		pid_t pid = atoi(dentry->d_name);
		if ( pid ) { // /proc/[pid]
			char path[1024] = {0};
			char data[2048] = {0};

			snprintf(path, sizeof(path), "/proc/%d/stat", pid);

			if ( get_proc_data(path, data, sizeof(data)) < 0 ) goto out;

			mco_cleanse(tmp, sizeof(tmp));
			// TODO: tmp[1] 값의 경우 긴 프로세스 이름은 짤리는 문제가 있음
			sscanf(data, "%s (%[^)]) %c %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
					tmp[0]  /* pid */,          tmp[1] /* shell */,      tmp[2]  /* stat */,     tmp[3]  /* ppid */, 
					tmp[4]  /* pgrp */,         tmp[5] /* session */,    tmp[6]  /* tty */,      tmp[7]  /* tty pgrp */,
					tmp[8]  /* flags */,        tmp[9] /* min flt */,    tmp[10] /* cmin flt */, tmp[11] /* maj flt */,
					tmp[12] /* cmaj flt */,    tmp[13] /* utime */,      tmp[14] /* stime */,    tmp[15] /* cutime */,
					tmp[16] /* cstime */,      tmp[17] /* priority */,   tmp[18] /* nice */,     tmp[19] /* timeout */,
					tmp[20] /* it_real_val */, tmp[21] /* start_time */, tmp[22] /* vsize */,    tmp[23] /* rss */,
					tmp[24] /* rlim */,        tmp[25] /* start_code */, tmp[26] /* end_code */, tmp[27] /* start_stack */,
					tmp[28] /* sp */,          tmp[29] /* pc */);


			// 현재의 /proc/[pid] 의 cmdline 값이 인자값인 pname 과 일치하는지 비교하여, 일치한다면 process 의 정보를 세팅해준다.
			if ( strcmp(tmp[1], pname) == 0 ) {
				pstat->alive = 1;
				snprintf(pstat->name, sizeof(pstat->name), "%s", tmp[1]);
				if ( (pstat->uid = mco_getUID(pid)) < 0 ) goto out;
				pstat->pid = pid;

				struct tm tm;
				if ( get_process_start_time(tmp[21], &tm) < 0 ) {
					goto out;
				}

				strftime(pstat->st_time, sizeof(pstat->st_time), "%m-%d %H:%M:%S", &tm);
				closedir(dirp);
				return 0; // found
			}
		}
	}
		
	MCO_SET_ERROR("not exist process: %s", pname);

	closedir(dirp);

	return 1; // not found

out:
	closedir(dirp);

	return -1;
}

/* 동일한 이름의 프로세스에 대한 정보를 저장하기위한 API */
int mco_get_processes_status(const char *pname, struct mco_process_status *pstats, int *size)
{
	int i = 0, found = 0;
	DIR *dirp = NULL;
	char tmp[32][64];

	mco_cleanse(pstats, sizeof(struct mco_process_status) * (*size));

	dirp = opendir("/proc");
	if ( !dirp ) {
		MCO_SET_ERROR("opendir() is failed: /proc (errmsg: %s errno: %d)\n", strerror(errno), errno);
		return -1;
	}

	for ( struct dirent *dentry = readdir(dirp); dentry; dentry = readdir(dirp) ) {
		pid_t pid = atoi(dentry->d_name);
		if ( pid ) { // /proc/[pid]
			char path[1024] = {0};
			char data[2048] = {0};

			snprintf(path, sizeof(path), "/proc/%d/stat", pid);

			if ( get_proc_data(path, data, sizeof(data)) < 0 ) goto out;

			mco_cleanse(tmp, sizeof(tmp));
			// TODO: tmp[1] 값의 경우 긴 프로세스 이름은 짤리는 문제가 있음
			sscanf(data, "%s (%[^)]) %c %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
					tmp[0]  /* pid */,          tmp[1] /* shell */,      tmp[2]  /* stat */,     tmp[3]  /* ppid */, 
					tmp[4]  /* pgrp */,         tmp[5] /* session */,    tmp[6]  /* tty */,      tmp[7]  /* tty pgrp */,
					tmp[8]  /* flags */,        tmp[9] /* min flt */,    tmp[10] /* cmin flt */, tmp[11] /* maj flt */,
					tmp[12] /* cmaj flt */,    tmp[13] /* utime */,      tmp[14] /* stime */,    tmp[15] /* cutime */,
					tmp[16] /* cstime */,      tmp[17] /* priority */,   tmp[18] /* nice */,     tmp[19] /* timeout */,
					tmp[20] /* it_real_val */, tmp[21] /* start_time */, tmp[22] /* vsize */,    tmp[23] /* rss */,
					tmp[24] /* rlim */,        tmp[25] /* start_code */, tmp[26] /* end_code */, tmp[27] /* start_stack */,
					tmp[28] /* sp */,          tmp[29] /* pc */);


			// 현재의 /proc/[pid] 의 cmdline 값이 인자값인 pname 과 일치하는지 비교하여, 일치한다면 process 의 정보를 세팅해준다.
			if ( strcmp(tmp[1], pname) == 0 ) {
				if ( i >= *size ) {
					MCO_SET_ERROR("buffer overflow");
					goto out;
				}

				pstats[i].alive = 1;
				snprintf(pstats[i].name, sizeof(pstats[i].name), "%s", tmp[1]);
				if ( (pstats[i].uid = mco_getUID(pid)) < 0 ) goto out;
				pstats[i].pid = pid;

				struct tm tm;
				if ( get_process_start_time(tmp[21], &tm) < 0 ) {
					goto out;
				}
				strftime(pstats[i].st_time, sizeof(pstats[i].st_time), "%m-%d %H:%M:%S", &tm);

				found = 1;
				i++;

				continue; // 동일한 이름의 프로세스를 계속 찾는다.
			}
		}
	}
		
	closedir(dirp);
		
	if ( found ) {
		*size = i;
		return 0;
	}
	else {
		MCO_SET_ERROR("not exist process: %s", pname);
		return 1; // not found
	}

out:
	closedir(dirp);
	return -1;
}

int mco_kill_process(pid_t pid)
{
	if ( kill(pid, SIGTERM) < 0 ) {
		if ( errno == ESRCH ) { // 이미 죽어있다.
			return 0;
		}
		else if ( errno == EPERM ) {
			MCO_SET_ERROR("kill() is failed: can't kill (permission deny)");
			return -1;
		}

		if ( kill(pid, SIGKILL) < 0) {
			MCO_SET_ERROR("kill() is failed: %s (errno: %d)", strerror(errno), errno);
			return -1;
		}
	}

	// usleep 필요한가..?

	// SIGTERM 으로 죽지 않았을 경우
	if ( mco_getUID(pid) > 0 ) {
		if ( kill(pid, SIGKILL) < 0 ) {
			MCO_SET_ERROR("kill() is failed: %s (errno: %d)", strerror(errno), errno);
			return -1;
		}
	}

	return 0;
}

int mco_kill_process_name(const char *pname)
{
	struct mco_process_status pstat;

	if ( mco_get_process_status(pname, &pstat) != 0 ) return -1;

	if ( mco_kill_process(pstat.pid) < 0 ) return -1;

	return 0;
}

int mco_killall_process_name(const char *pname)
{
	int size = KILLALL_MAX;
	struct mco_process_status pstats[KILLALL_MAX];

	if ( mco_get_processes_status(pname, pstats, &size) < 0 ) return -1;

	for ( int i = 0; i < size; i++ ) {
		if ( mco_kill_process(pstats[i].pid) < 0 ) return -1;
	}
	
	return 0;
}

int mco_killall_owner_process_name(uid_t uid, const char *pname)
{
	int size = KILLALL_MAX;
	struct mco_process_status pstats[KILLALL_MAX];

	if ( mco_get_processes_status(pname, pstats, &size) < 0 ) return -1;

	for ( int i = 0; i < size; i++ ) {
		if ( pstats[i].uid == uid ) { // owner 인 경우만 kill
			if ( mco_kill_process(pstats[i].pid) < 0 ) return -1;
		}
	}
	
	return 0;
}

// dpath file force write
int mco_copy_file(const char *spath, const char *dpath, mode_t mode)
{
	int same_path = 0;
	char dpath2[1024] = {0};
	long slen;
	FILE *sfp = NULL, *dfp = NULL;

	if ( !spath || !dpath ) {
		MCO_SET_ERROR("invalid argument - spath: %s, dpath: %s", spath ? spath : "NULL", dpath ? dpath : "NULL");
		goto out;
	}

	// 복사할 경로가 원본과 같을 경우(원본을 삭제하고, 덮어쓸것이다.)
	if ( strcmp(spath, dpath) == 0 ) {
		snprintf(dpath2, sizeof(dpath2), "%s.tmp", dpath);
		same_path = 1;
	}
	else {
		snprintf(dpath2, sizeof(dpath2), "%s", dpath);
	}

	if ( (slen = mco_get_file_len(spath)) < 0 ) goto out;

	sfp = fopen(spath, "rb");
	if ( !sfp ) {
		MCO_SET_ERROR("fopen() is failed: %s (errmsg: %s, errno: %d)", spath, strerror(errno), errno);
		goto out;
	}
	
	dfp = fopen(dpath2, "wb");
	if ( !dfp ) {
		MCO_SET_ERROR("fopen() is failed: %s (errmsg: %s, errno: %d)", spath, strerror(errno), errno);
		goto out;
	}

	ssize_t nread, nwrite, tnwrite = 0;
	while ( slen > 0 ) {
		unsigned char buf[BUFSIZ]; // 버퍼크기만큼 나누어서 읽기위함

		nread = fread(buf, sizeof(unsigned char), sizeof(buf), sfp);
		switch (nread) {
			case -1:
				MCO_SET_ERROR("fread() is failed: %s (errmsg: %s, errno: %d)", spath, strerror(errno), errno);
				goto out;
			case 0:
				break;
			default:
				nwrite = fwrite(buf, sizeof(unsigned char), nread, dfp);
				if ( nwrite < 0 ) {
					MCO_SET_ERROR("fwrite() is failed: %s (errmsg: %s, errno: %d)", dpath2, strerror(errno), errno);
					goto out;
				}
				tnwrite += nwrite;
				slen -= nread;
		}
	}

	// 같은 경로일경우 원본 파일을 삭제하고, .tmp 로 되어있는 사본 파일을 원본 파일명으로 rename 한다.
	if ( same_path ) {
		if ( unlink(spath) < 0 ) {
			MCO_SET_ERROR("unlink() is failed: %s (errmsg: %s, errno: %d)", spath, strerror(errno), errno);
			goto out;
		}
		if ( rename(dpath2, spath) < 0 ) {
			MCO_SET_ERROR("rename() is failed: old - %s, new - %s (errmsg: %s, errno: %d)", dpath2, spath, strerror(errno), errno);
			goto out;
		}
	}

	if ( mode > 0 ) (void)chmod(dpath2, mode);

	fclose(sfp);
	fclose(dfp);

	return 0;

out:
	safe_fclose(sfp);
	safe_fclose(dfp);

	return -1;
}
