#include "mco_string.h"
#include "mco_hidden.h"

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
