#ifndef FREQLIB_FREQ_FREQ_ITEM_H_
#define FREQLIB_FREQ_FREQ_ITEM_H_
#include <stdint.h>
#include <string>

#pragma pack(1)
namespace libwxfreq {

struct FreqKey {
  char key[32];
  uint32_t appid;
};

struct FreqItem {
  unsigned int level1_cnt;
  unsigned int level2_cnt;
  unsigned int level3_cnt;
};

}  // namespace libwxfreq
#pragma pack()
#endif  // FREQLIB_FREQ_FREQ_ITEM_H_
