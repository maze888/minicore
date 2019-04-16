#include "mco_string.h"
#include "mco_hidden.h"

static int get_token_count(char *buf, const char *delim)
{
	int tcnt = 0;
	char *tbuf, *token, *save_token;

	tbuf = strdup(buf);
	if ( !tbuf ) {
		mco_set_last_error("strdup() is failed: %s (errno: %d)", strerror(errno), errno);
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
	size_t i, dest_len, src_len;

	if ( !dest || !src || dsize <= 2 ) {
		mco_set_last_error("invalid argument - dest: %s, src: %s, dsize: %d", dest ? dest : "NULL", src ? src : "NULL", dsize);
		return 0;
	}

	src_len = strlen(src);
	dest_len = strlen(dest);
	if ( dest_len + src_len >= dsize ) {
		mco_set_last_error("not enough buffer");
		return 0;
	}

	for ( i = 0; src[i]; i++ ) dest[dest_len + i] = src[i];

	dest[dest_len + i] = '\0';

	return dest_len + i;
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
	char *p;
	size_t len, trim = 0;

	if ( !buf ) {
		mco_set_last_error("invalid argument - buf: NULL");
		return 0;
	}

	len = strlen(buf);

	for ( p = buf; isspace(*p); p++, trim++ );
		
	memcpy(buf, p, len - trim);

	buf[len - trim] = '\0';

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
		mco_set_last_error("invalid argument - buf: NULL");
		return 0;
	}

	len = strlen(buf);

	for ( p = buf + len - 1; isspace(*p); p--, trim++ );

	buf[len - trim] = '\0';

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
		mco_set_last_error("invalid argument - buf: %s, bsize: %lu, fstr: %s, rstr: %s", buf ? buf : "NULL", bsize, fstr ? fstr : "NULL", rstr ? rstr : "NULL");
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
			mco_set_last_error("not enough buffer");
			goto out;
		}
	}
	
	// protect memory overlap
	tmp_buf = (char *)malloc(bsize);
	if ( !tmp_buf ) {
		mco_set_last_error("calloc() is failed: %s (errno: %d)", strerror(errno), errno);
		goto out;
	}

	char *p = buf;
	for ( int i = 0; i < buf_len; i++, p++) {
		if ( buf[i] == fstr[0] ) {
			if ( memcmp(p, fstr, fstr_len) == 0 ) {
				memset(tmp_buf, 0x00, bsize);

				if ( fstr_len == rstr_len ) {
					memcpy(p, rstr, rstr_len);
				}
				else if ( fstr_len > rstr_len ) {
					size_t plen = strlen(p + fstr_len);

					memcpy(tmp_buf, p + fstr_len, plen);
					memcpy(p + rstr_len, tmp_buf, plen);
					memcpy(p, rstr, rstr_len);

					p[plen + 1] = '\0';
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
