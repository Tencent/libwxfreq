/*
* Tencent is pleased to support the open source community by making libwxfreq available.
*
* Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the BSD 3-Clause License (the "License"); you may not use this file
* except in compliance with the License. You may obtain a copy of the License at
*
*               https://opensource.org/licenses/BSD-3-Clause
*
* Unless required by applicable law or agreed to in writing, software distributed
* under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
* CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
* language governing permissions and limitations under the License.
*/

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
