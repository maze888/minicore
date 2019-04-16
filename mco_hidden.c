#include "mco_hidden.h"

char mco_last_error[1024];

/**
 * @brief   Save error that occurred in library
 */
void mco_set_last_error(const char *func, char *fmt, ...)
{
	va_list ap;
	char buf[1024] = {0};

	memset(mco_last_error, 0x00, sizeof(mco_last_error));

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	snprintf(mco_last_error, sizeof(mco_last_error), "%s(): %s", func, buf);
}
