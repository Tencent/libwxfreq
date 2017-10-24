#ifndef FREQLIB_UTIL_SHM_FREQ_STAT_H_
#define FREQLIB_UTIL_SHM_FREQ_STAT_H_
#include <pthread.h>
#include <map>
#include <string>
#include "freq_stat.h"
#include "multi_hash_table.h"

namespace libwxfreq {
class FreqItem;

class ShmFreqStat : public FreqStat {
 public:
  ShmFreqStat(bool zero_init, key_t key, unsigned int item_cnt);
  virtual uint32_t& GetItem(const std::string& key, const uint32_t appid,
                            const uint32_t cnt, FreqItem* &freq_item);
  virtual bool TryReload();
  virtual ~ShmFreqStat();

 private:
  typedef bool (*cmp) (const FreqKey&a, const FreqKey&b);
  typedef std::map<FreqKey, FreqItemForShm, cmp> ItemMap;
  void ShrinkMap(time_t now);
  ItemMap item_map_;
  pthread_rwlock_t lock_;
  unsigned int shrink_;
  unsigned int last_shrink_time_;
  unsigned int count_;
  MultiHashTable table_;
};

}  // namespace libwxfreq

#endif  // FREQLIB_UTIL_SHM_FREQ_STAT_H_
