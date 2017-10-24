#ifndef FREQLIB_UTIL_LOG_H_
#define FREQLIB_UTIL_LOG_H_

namespace libwxfreq {

typedef int (*LogFunc)(const char* format, ...);

extern LogFunc gLog;

}  // namespace libwxfreq
#endif  // FREQLIB_UTIL_LOG_H_
