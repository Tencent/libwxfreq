#ifndef FREQLIB_UTIL_RELOAD_H_
#define FREQLIB_UTIL_RELOAD_H_
#include <stdint.h>
namespace libwxfreq {

typedef int (*LoadFunc)(const char* type_name);
typedef int (*DumpFunc)(const char* opname, const char* type_name,
                        const char *key, const uint32_t appid,
                        const uint32_t linger_time,
                        const uint32_t block_level);

extern LoadFunc gLoadFunc;
extern DumpFunc gDumpFunc;

}  // namespace libwxfreq
#endif  // FREQLIB_UTIL_RELOAD_H_
