#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

void shutdown(void) {
}

void vwarnx(char *fmt, va_list vl) {
  vfprintf(stderr, fmt, vl);
}

void warnx(char *fmt, va_list vl) {
  vfprintf(stderr, fmt, vl);
}

_Noreturn void errx(int s, char *fmt, va_list vl) {
  warnx(fmt, vl);
  exit(s);
}

_Noreturn void err(int s, char *fmt, va_list vl) {
  errx(s, fmt, vl);
}

#else
#error "you fool! don't use that!"
#endif
