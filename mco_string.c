/**
 * @file    mco_string.c
 * @brief   문자열 처리 함수 정의
 * @date    2019.09.20
 * @author  naze@dreamsecurity.com
 *
 */

#include "mco_string.h"
#include "mco_inter.h"

static int get_token_count(char *buf, const char *delim)
{
	int tcnt = 0;
	char *tbuf, *token, *save_token;

	tbuf = strdup(buf);
	if ( !tbuf ) {
		MCO_SET_ERROR("strdup() is failed: %s (errno: %d)", strerror(errno), errno);
		return -1;
	}

	for ( token = strtok_r(tbuf, delim, &save_token); token; token = strtok_r(NULL, delim, &save_token) ) tcnt++;

	free(tbuf);

	return tcnt;
}

/**
 * @brief   Add string to buffer
 *
 * @param   dest   buffer
 * @param   src    string
 * @param   dsize  size of dest buffer
 *
 * @return  Length of string in buffer
 */
size_t mco_strncat(char *dest, const char *src, size_t dsize)
{
	size_t dest_len, src_len;

	if ( !dest || !src || dsize <= 2 ) {
		MCO_SET_ERROR("invalid argument - dest: %s, src: %s, dsize: %d", dest ? dest : "NULL", src ? src : "NULL", dsize);
		return 0;
	}

	src_len = strlen(src);
	dest_len = strlen(dest);
	if ( dest_len + src_len >= dsize ) {
		MCO_SET_ERROR("not enough buffer");
		return 0;
	}

	memcpy(dest + dest_len, src, src_len);

	return dest_len + src_len;
}

/**
 * @brief   Left trim from buffer
 *
 * @param   buf  buffer
 *
 * @return  Length of trimed buffer
 */
size_t mco_left_trim(char *buf)
{
	if ( !buf ) {
		MCO_SET_ERROR("invalid argument - buf: NULL");
		return 0;
	}

	char *buf_dup = strdup(buf);
	if ( !buf_dup ) {
		MCO_SET_ERROR("strdup() is failed: %s (errno: %d)", strerror(errno), errno);
		return 0;
	}

	size_t len = strlen(buf);

	char *p;
	size_t trim;
	for ( p = buf, trim = 0; isspace(*p); p++ ) trim++;
		
	// protect memory overlap
	memcpy(buf_dup, p, len - trim);
	memcpy(buf, buf_dup, len - trim);

	buf[len - trim] = '\0';

	free(buf_dup);

	return trim;
}

/**
 * @brief   Right trim from buffer
 *
 * @param   buf  buffer
 *
 * @return  Length of trimed buffer
 */
size_t mco_right_trim(char *buf)
{
	char *p;
	size_t len, trim = 0;

	if ( !buf ) {
		MCO_SET_ERROR("invalid argument - buf: NULL");
		return 0;
	}

	len = strlen(buf);

	for ( p = buf + len - 1; isspace(*p); p--, trim++ );

	buf[len - trim] = '\0';

	return trim;
}

/**
 * @brief   Left / Right trim from buffer
 *
 * @param   buf  buffer
 *
 * @return  Length of trimed buffer
 */
size_t mco_trim(char *buf)
{
	size_t trim, ltrim, rtrim;

	ltrim = mco_left_trim(buf);
	rtrim = mco_right_trim(buf);

	trim = ltrim + rtrim;

	return trim;
}

/**
 * @brief   Replace string of buffer
 *
 * @param   buf    buffer
 * @param   bsize  buffer size
 * @param   fstr   string to be substituted
 * @param   rstr   substituted string
 *
 * @return  Length of string in buffer
 */
size_t mco_replace_str(char *buf, size_t bsize, const char *fstr, const char *rstr)
{
	char *tmp_buf = NULL;
	size_t buf_len, fstr_len, rstr_len;

	if ( !buf || bsize < 2 || !fstr || !rstr ) {
		MCO_SET_ERROR("invalid argument - buf: %s, bsize: %lu, fstr: %s, rstr: %s", buf ? buf : "NULL", bsize, fstr ? fstr : "NULL", rstr ? rstr : "NULL");
		goto out;
	}

	buf_len = strlen(buf);
	fstr_len = strlen(fstr);
	rstr_len = strlen(rstr);
				
	// protect buffer overflow
	if ( fstr_len < rstr_len ) {
		int fcnt = get_token_count(buf, fstr);
		if ( fcnt < 0 ) goto out;
		
		if ( bsize <= (buf_len + (rstr_len - fstr_len) * fcnt) ) {
			MCO_SET_ERROR("not enough buffer");
			goto out;
		}
	}
	
	// protect memory overlap
	tmp_buf = (char *)malloc(bsize);
	if ( !tmp_buf ) {
		MCO_SET_ERROR("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}

	char *p = buf;
	for ( int i = 0; i < buf_len; i++, p++) {
		if ( buf[i] == fstr[0] ) {
			if ( memcmp(p, fstr, fstr_len) == 0 ) {
				mco_cleanse(tmp_buf, bsize);

				if ( fstr_len == rstr_len ) {
					memcpy(p, rstr, rstr_len);
				}
				else if ( fstr_len > rstr_len ) {
					size_t plen = strlen(p + fstr_len);

					// copy tail
					memcpy(tmp_buf, p + fstr_len, plen);

					if ( rstr_len == 0 ) { // rstr is ""
						memcpy(p, tmp_buf, plen);
						p[plen] = '\0';
					}
					else {
						memcpy(p + rstr_len, tmp_buf, plen);
						memcpy(p, rstr, rstr_len);
						p[plen + 1] = '\0';
					}
				}
				else if ( fstr_len < rstr_len ) {
					size_t plen = strlen(p + fstr_len);

					memcpy(tmp_buf, p + fstr_len, plen);
					memcpy(p + rstr_len, tmp_buf, plen);
					memcpy(p, rstr, rstr_len);
					
					// buffer is longer
					buf_len = buf_len + rstr_len - fstr_len;
					p[plen + rstr_len] = '\0';
				}
			}
		}
	}
	
	safe_free(tmp_buf);
	
	return buf_len;
	
out:
	safe_free(tmp_buf);

	return 0;
}

/**
 * @brief   Convert to upper case
 *
 * @param   buf    buffer to be converted to uppercase
 */
void mco_cnvt_uppercase(char *buf)
{
	for ( size_t i = 0, len = strlen(buf); i < len; i++ ) buf[i] = toupper(buf[i]);
}

/**
 * @brief   Convert to lower case
 *
 * @param   buf    buffer to be converted to lowercase
 */
void mco_cnvt_lowercase(char *buf)
{
	for ( size_t i = 0, len = strlen(buf); i < len; i++ ) buf[i] = tolower(buf[i]);
}

void mco_generate_random_string(const char *pattern, char *buf, size_t bsize)
{
	size_t i, pattern_len = strlen(pattern);

	srand(rand());

	for ( i = 0; i < bsize; i++) {
		buf[i] = pattern[rand() % pattern_len];
	}
}

/**
 * @brief   Generate date format string
 *
 * @param   buf    buffer to be saved in date format
 * @param   bsize  buffer size
 */
void mco_get_current_date(char *buf, size_t bsize)
{
	time_t t;
	struct tm tm;

	mco_cleanse(&tm, sizeof(struct tm));

	t = time(NULL);
	localtime_r(&t, &tm);

	strftime(buf, bsize, "%F %T", &tm);
}

/**
 * @brief   Generate datetime format string
 *
 * @param   buf    buffer to be saved in datetime format
 * @param   bsize  buffer size
 */
void mco_get_current_datetime(char *buf, size_t bsize)
{
	time_t t;
	struct tm tm;
	struct timeval tv;
	char date[128] = {0};

	mco_cleanse(&tm, sizeof(tm));
	mco_cleanse(&tv, sizeof(tv));

	gettimeofday(&tv, NULL);

	t = tv.tv_sec;
	localtime_r(&t, &tm);
	
	strftime(date, sizeof(date), "%F %T", &tm);
	snprintf(buf, bsize, "%s.%ld", date, tv.tv_usec);
}
