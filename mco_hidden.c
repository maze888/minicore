#include "mco_hidden.h"

char mco_last_error[1024];

/**
 * @brief   Save error that occurred in library
 */
void mco_set_last_error(char *fmt, ...)
{
	va_list ap;

	memset(mco_last_error, 0x00, sizeof(mco_last_error));

	va_start(ap, fmt);
	vsnprintf(mco_last_error, sizeof(mco_last_error), fmt, ap);
	va_end(ap);
}

