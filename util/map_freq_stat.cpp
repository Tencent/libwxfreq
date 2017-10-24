#include "map_freq_stat.h"
#include <cstring>
#include "reload.h"

namespace libwxfreq {
static uint32_t dummy_timestamp = 0;

bool FreqKeyCmp(const FreqKey&a, const FreqKey&b) {
  if (a.appid < b.appid) {
    return true;
  } else if (a.appid == b.appid) {
    return strncmp(a.key, b.key, sizeof(a.key)) < 0;
  }
  return false;
}

uint32_t& MapFreqStat::GetItem(const std::string& key, const uint32_t appid,
                               const uint32_t cnt, FreqItem* &freq_item) {
  FreqKey freqkey;
  freqkey.appid = appid;
  strncpy(freqkey.key, key.c_str(), sizeof(freqkey.key));
  if (cnt != 0) {
    FreqItemForMap& freqitemformap = item_map[freqkey];
    freq_item = reinterpret_cast<FreqItem*>(&freqitemformap);
    return freqitemformap.timestamp;
  } else {
    ItemMap::iterator it = item_map.find(freqkey);
    if (it == item_map.end()) {
      freq_item = NULL;
      return dummy_timestamp;
    } else {
      freq_item = reinterpret_cast<FreqItem*>(&(it->second));
      return it->second.timestamp;
    }
  }
}

bool MapFreqStat::TryReload() {
  return gLoadFunc(type_name_.c_str()) == 0;
}

}  // namespace libwxfreq
