#ifndef _MCO_HIDDEN_H_
#define _MCO_HIDDEN_H_

#include "mco.h"

#define MCO_SET_ERROR(...) mco_set_last_error(__func__, __VA_ARGS__);

void mco_set_last_error(const char *func, char *fmt, ...);

#endif
