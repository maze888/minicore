/**
 * @file    mco_util.c
 * @brief   유틸성 함수 정의
 * @date    2019.09.20
 * @author  naze@dreamsecurity.com
 *
 */

#include "mco_util.h"
#include "mco_inter.h"

long mco_get_file_len(const char *path)
{
	if ( !path ) {
		MCO_SET_ERROR("invalid argument - path: %s", path ? path : "NULL");
		return -1;
	}

	FILE *fp = fopen(path, "rb");
	if ( !fp ) {
		MCO_SET_ERROR("fopen() is failed: %s (errmsg: %s, errno: %d)", path, strerror(errno), errno);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);

	long len = ftell(fp);

	fclose(fp);

	return len;
}

void mco_hex_print(FILE *fp, const char *subject, const unsigned char *bin, int len)
{
	int len_16d = len / 16;

	fprintf(fp, "%s - %d bytes\r\n", subject, len);
	for ( int j = 0; j <= len_16d && len > 0; j++ ) {
		fprintf(fp, "  %04d - ", j + 1);

		const unsigned char *bin_data = bin + j * 16;
		for ( int k = 0; k < 16 && len > 0; k++, bin_data++, len-- ) {
			if ( k != 0 && k % 4 == 0 ) fprintf(fp, " ");

			if ( *bin_data < 0x10 ) fprintf(fp, "0%X", *bin_data);
			else fprintf(fp, "%X", *bin_data);
		}
		fprintf(fp, "\r\n");
	}
}

void mco_hex_cprint(const char *subject, const unsigned char *bin, int len)
{
	mco_hex_print(stderr, subject, bin, len);
}
