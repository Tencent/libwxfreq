#include "options.h"

namespace libwxfreq {

extern LogFunc gLog;
extern LoadFunc gLoadFunc;
extern DumpFunc gDumpFunc;

void SetLog(LogFunc log) {
  gLog = log;
}

void SetLoadFunc(LoadFunc func) {
  gLoadFunc = func;
}

void SetDumpFunc(DumpFunc func) {
  gDumpFunc = func;
}

}  // namespace libwxfreq
