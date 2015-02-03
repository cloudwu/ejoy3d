#include "log.h"
#include <stdarg.h>
#include <stdio.h>

#define MAX_LENGTH 4096

static void (*LOGCB)(void *ud, const char *msg) = NULL;

void
log_setcallback(void (*cb)(void *ud, const char *msg)) {
	LOGCB = cb;
}

void
log_printf(void *ud, const char * format, ...) {
	if (LOGCB) {
		char tmp[MAX_LENGTH];
		int n;
		va_list ap;
		va_start(ap, format);
		n = vsnprintf(tmp, MAX_LENGTH-1, format, ap);
		if (n < 0 || n >= MAX_LENGTH-1) {
			tmp[MAX_LENGTH-1] = '\0';
		}
		va_end(ap);
		LOGCB(ud, tmp);
	}
}
