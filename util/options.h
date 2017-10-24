#ifndef FREQLIB_UTIL_OPTIONS_H_
#define FREQLIB_UTIL_OPTIONS_H_
#include "log.h"
#include "reload.h"

namespace libwxfreq {

void SetLog(LogFunc log);

void SetLoadFunc(LoadFunc func);
void SetDumpFunc(DumpFunc func);

}  // namespace libwxfreq

#endif  // FREQLIB_UTIL_OPTIONS_H_
