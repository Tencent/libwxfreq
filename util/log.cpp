#include "log.h"
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

namespace libwxfreq {

static const char* kDefaultLogFileName = "/tmp/libwxfreq.log";

static inline pid_t gettid() {
  return syscall(SYS_gettid);
}

static void Logv(FILE *file, const char* format, va_list ap) {
  const uint64_t thread_id = gettid();

  char buffer[500];
  for (int iter = 0; iter < 2; iter++) {
    char* base;
    int bufsize;
    if (iter == 0) {
      bufsize = sizeof(buffer);
      base = buffer;
    } else {
      bufsize = 30000;
      base = new char[bufsize];
    }
    char* p = base;
    char* limit = base + bufsize;

    struct timeval now_tv;
    gettimeofday(&now_tv, NULL);
    const time_t seconds = now_tv.tv_sec;
    struct tm t;
    localtime_r(&seconds, &t);
    p += snprintf(p, limit - p,
                  "%04d/%02d/%02d-%02d:%02d:%02d.%06d %llx ",
                  t.tm_year + 1900,
                  t.tm_mon + 1,
                  t.tm_mday,
                  t.tm_hour,
                  t.tm_min,
                  t.tm_sec,
                  static_cast<int>(now_tv.tv_usec),
                  static_cast<uint64_t>(thread_id));

    // Print the message
    if (p < limit) {
      va_list backup_ap;
      va_copy(backup_ap, ap);
      p += vsnprintf(p, limit - p, format, backup_ap);
      va_end(backup_ap);
    }

    // Truncate to available space if necessary
    if (p >= limit) {
      if (iter == 0) {
        continue;       // Try again with larger buffer
      } else {
        p = limit - 1;
      }
    }

    // Add newline if necessary
    if (p == base || p[-1] != '\n') {
      *p++ = '\n';
    }

    assert(p <= limit);
    fwrite(base, 1, p - base, file);
    fflush(file);
    if (base != buffer) {
      delete[] base;
    }
    break;
  }
}

static int Log(const char* format, ...) {
  static FILE* file = fopen(kDefaultLogFileName, "a");
  if (file == NULL) return -1;
  va_list ap;
  va_start(ap, format);
  Logv(file, format, ap);
  va_end(ap);
  return 0;
}

LogFunc gLog = Log;

}  //  namespace libwxfreq

