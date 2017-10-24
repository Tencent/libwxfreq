#ifndef FREQLIB_UTIL_MAP_FREQ_STAT_H_
#define FREQLIB_UTIL_MAP_FREQ_STAT_H_
#include <map>
#include <string>
#include "freq_item.h"
#include "freq_stat.h"

namespace libwxfreq {
#pragma pack(1)
struct FreqItemForMap : public FreqItem {
  uint32_t timestamp;
};
#pragma pack()
bool FreqKeyCmp(const FreqKey&a, const FreqKey&b);

class MapFreqStat : public FreqStat {
 public:
  MapFreqStat() : item_map(FreqKeyCmp) { }
  virtual uint32_t& GetItem(const std::string& key, const uint32_t appid,
                            const uint32_t cnt, FreqItem* &freq_item);
  virtual bool TryReload();

 private:
  typedef bool (*cmp) (const FreqKey&a, const FreqKey&b);
  typedef std::map<FreqKey, FreqItemForMap, cmp> ItemMap;
  ItemMap item_map;
};

}  // namespace libwxfreq

#endif  // FREQLIB_UTIL_MAP_FREQ_STAT_H_
