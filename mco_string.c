#include "mco_string.h"

size_t mco_strncat(char *dest, const char *src, size_t dsize)
{
	size_t i, dest_len, src_len;

	if ( !dest || !src || dsize <= 2 ) {
		mco_set_last_error("invalid argument - dest: %s, src: %s, dsize: %d", dest ? dest : "NULL", src ? src : "NULL", dsize);
		return 0;
	}

	src_len = strlen(src);
	dest_len = strlen(dest);
	if ( dest_len + src_len >= dsize ) return 0;

	for ( i = 0; src[i]; i++ ) {
		dest[dest_len + i] = src[i];
	}

	dest[dest_len + i] = '\0';

	return dest_len + i;
}
