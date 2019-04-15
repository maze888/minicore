#include "mco.h"

/**
 * @brief   Return error that occurred in library
 *
 * @return  Last error in that occrued in library
 *
 */
const char * mco_get_last_error()
{
	return mco_last_error;
}

/**
 * @brief   Print hex values through standard output
 *
 * @param   p       buffer
 * @param   p_size  size of buffer
 *
 */
void mco_hex_print(unsigned char *p, int p_size)
{
	if ( !p && p_size <= 0 ) return;

	printf("---------------------- hex print ----------------------\n");

	int new_line = 1;
	for ( int i = 1, seq = 0; i <= p_size; i++ ) {
		if ( new_line ) {
			printf("%4d: ", seq);
			new_line = 0;
		}

		if ( p[i-1] < 0x10 ) printf("0x0%x ", p[i-1]);
		else printf("0x%x ", p[i-1]);

		if ( i % 8 == 0 ) {
			seq++;
			new_line = 1;
			printf("\n");
		}
	}
	printf("\n");

	fflush(stdout);
}
