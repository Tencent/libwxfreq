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
