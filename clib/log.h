#ifndef ejoy3d_log_h
#define ejoy3d_log_h

void log_setcallback(void (*cb)(void *ud, const char *msg));
void log_printf(void *ud, const char * format, ...);

#endif
