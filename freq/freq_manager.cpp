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

#include "freq_manager.h"
#include "freq_item.h"
#include "appid.h"
#include "freq_stat.h"
#include "log.h"
#include "appid_manager.h"

namespace libwxfreq {

TypeName2FreqStatMap FreqManager::type_name_map_;
const uint32_t FreqManager::kWhiteFlag = 0xffffffff;
const uint32_t FreqManager::kAddBlockFlag = 0xfffffffe;
static AppidMeta dummy_appid_meta;

inline uint32_t FreqManager::TimeSplit(time_t timestamp, uint32_t scale) {
  return timestamp/scale;
}

bool FreqManager::Init(const TypeName2FreqStatMap& type_name_map) {
  TypeName2FreqStatMap::iterator it = type_name_map_.begin();
  while (it != type_name_map_.end()) {
    delete it->second;
    it->second = NULL;
    ++it;
  }
  type_name_map_ = type_name_map;
  it = type_name_map_.begin();

  while (it != type_name_map_.end()) {
    if (it->second->TryReload() == false ) {
      gLog("[%s][%d]: ERROR type name %s TryReload failed\n",
          __FILE__, __LINE__, it->first.c_str());
    }
    ++it;
  }
  return true;
}

const FreqItem* FreqManager::CommAdd(FreqStat *freq_stat,
                                    const std::string& key,
                                    const AppidMeta& appid_meta,
                                    const uint32_t appid,
                                    const uint32_t cnt) {
  FreqItem *freq_item = NULL;
  uint32_t& last_time = freq_stat->GetItem(key, appid, cnt, freq_item);
  if (freq_item == NULL)  {
    gLog("[%s][%d]: appid %u key %s GetItem empty\n", __FILE__, __LINE__,
        appid, key.c_str());
    return NULL;
  }

  if (freq_item->level1_cnt == kWhiteFlag) {
    gLog("[%s][%d]: appid %u key %s white\n", __FILE__, __LINE__,
        appid, key.c_str());
    return freq_item;
  } else if (freq_item->level1_cnt == kAddBlockFlag) {
    gLog("[%s][%d]: appid %u key %s blocked\n", __FILE__, __LINE__,
        appid, key.c_str());
    return freq_item;
  }

  time_t current_time = time(NULL);
  uint32_t current_interval_level3 = TimeSplit(current_time,
                                                appid_meta.max_interval());
  uint32_t current_interval_level2 = TimeSplit(current_time,
                                                appid_meta.mid_interval());
  uint32_t current_interval_level1 = TimeSplit(current_time,
                                              appid_meta.min_interval());

  uint32_t last_interval_level3 = TimeSplit(last_time,
                                            appid_meta.max_interval());
  uint32_t last_interval_level2 = TimeSplit(last_time,
                                            appid_meta.mid_interval());
  uint32_t last_interval_level1 = TimeSplit(last_time,
                                            appid_meta.min_interval());

  if (last_interval_level1 != current_interval_level1) {
    freq_item->level1_cnt = cnt;
  } else {
    __sync_fetch_and_add(&freq_item->level1_cnt, cnt);
  }

  if (last_interval_level2 != current_interval_level2) {
    freq_item->level2_cnt = cnt;
  } else {
    __sync_fetch_and_add(&freq_item->level2_cnt, cnt);
  }

  if (last_interval_level3 != current_interval_level3) {
    freq_item->level3_cnt = cnt;
  } else {
    __sync_fetch_and_add(&freq_item->level3_cnt, cnt);
  }
  last_time = current_time;
  return freq_item;
}

const FreqItem* FreqManager::Add(const char* type_name,
                                const std::string& key,
                                const uint32_t appid,
                                const uint32_t cnt) {
  if (type_name == NULL) {
    gLog("[%s][%d]: type name empty\n", __FILE__, __LINE__);
    return NULL;
  }
  if (appid == 0) {
    gLog("[%s][%d]: appid = 0\n", __FILE__, __LINE__);
    return NULL;
  }
  TypeName2FreqStatMap::iterator it = type_name_map_.find(type_name);
  if (it == type_name_map_.end()) {
    gLog("[%s][%d]: type name %s not exists\n", __FILE__, __LINE__, type_name);
    return NULL;
  }

  const AppidMeta *meta = AppidManager::GetAppidMeta(appid);
  if (meta == NULL) {
    return CommAdd(it->second, key, dummy_appid_meta, appid, cnt);
  } else {
    return CommAdd(it->second, key, *meta, appid, cnt);
  }
}

bool FreqManager::AddWhite(const char* type_name, const std::string& key,
                          const uint32_t appid, const uint32_t linger_time) {
  if (type_name == NULL) {
    gLog("[%s][%d]: type name empty\n", __FILE__, __LINE__);
    return false;
  }
  if (appid == 0) {
    gLog("[%s][%d]: appid = 0\n", __FILE__, __LINE__);
    return false;
  }
  TypeName2FreqStatMap::iterator it = type_name_map_.find(type_name);
  if (it == type_name_map_.end()) {
    gLog("[%s][%d]: type name %s not exists\n", __FILE__, __LINE__, type_name);
    return false;
  }
  FreqStat *freq_stat = it->second;

  if (freq_stat == NULL) {
    gLog("[%s][%d]: type name %s freq_stat NULL\n",
        __FILE__, __LINE__, type_name);
    return false;
  }

  FreqItem *freq_item = NULL;
  uint32_t& last_time = freq_stat->GetItem(key, appid, 1, freq_item);
  if (freq_item == NULL)  {
    gLog("[%s][%d]: appid %u key %s GetItem empty\n",
        __FILE__, __LINE__, appid, key.c_str());
    return false;
  }

  unsigned int expired_time = time(NULL) + linger_time;
  freq_item->level1_cnt = kWhiteFlag;
  last_time = expired_time - 90000;
  return true;
}

bool FreqManager::DeleteWhite(const char* type_name, const std::string& key,
                              const uint32_t appid) {
  if (type_name == NULL) {
    gLog("[%s][%d]: type name empty\n", __FILE__, __LINE__);
    return false;
  }

  TypeName2FreqStatMap::iterator it = type_name_map_.find(type_name);
  if (it == type_name_map_.end()) {
    gLog("[%s][%d]: type name %s not exists\n", __FILE__, __LINE__, type_name);
    return false;
  }
  FreqStat *freq_stat = it->second;

  if (freq_stat == NULL) {
    gLog("[%s][%d]: type name %s freq_stat NULL\n",
        __FILE__, __LINE__, type_name);
    return false;
  }

  FreqItem *freq_item = NULL;
  uint32_t& last_time = freq_stat->GetItem(key, appid, 0, freq_item);
  if (freq_item == NULL)  {
    return true;
  }

  if (GetWhiteLevel(freq_item) > 0) {
    last_time = 1; //expired
  }
  return true;
}

bool FreqManager::AddBlock(const char* type_name, const std::string& key,
                          const uint32_t appid, const uint32_t linger_time,
                          const uint32_t block_level) {
  if (type_name == NULL) {
    gLog("[%s][%d]: type name empty\n", __FILE__, __LINE__);
    return false;
  }

  TypeName2FreqStatMap::iterator it = type_name_map_.find(type_name);
  if (it == type_name_map_.end()) {
    gLog("[%s][%d]: type name %s not exists\n", __FILE__, __LINE__, type_name);
    return false;
  }
  FreqStat *freq_stat = it->second;


  FreqItem *freq_item = NULL;
  uint32_t& last_time = freq_stat->GetItem(key, appid, 1, freq_item);
  if (freq_item == NULL)  {
    gLog("[%s][%d]: appid %d key %s GetItem empty\n",
        __FILE__, __LINE__, appid, key.c_str());
    return false;
  }

  if (freq_stat == NULL) {
    gLog("[%s][%d]: type name %s freq_stat NULL\n",
        __FILE__, __LINE__, type_name);
    return false;
  }

  if (GetWhiteLevel(freq_item) > 0) {
    return true;
  }

  unsigned int expired_time = time(NULL) + linger_time;
  freq_item->level1_cnt = kAddBlockFlag;
  freq_item->level2_cnt = block_level;
  last_time = expired_time - 90000;
  return true;
}

bool FreqManager::DeleteBlock(const char* type_name, const std::string& key,
                              const uint32_t appid) {
  if (type_name == NULL) {
    gLog("[%s][%d]: type name empty\n", __FILE__, __LINE__);
    return false;
  }

  TypeName2FreqStatMap::iterator it = type_name_map_.find(type_name);
  if (it == type_name_map_.end()) {
    gLog("[%s][%d]: type name %s not exists\n", __FILE__, __LINE__, type_name);
    return false;
  }
  FreqStat *freq_stat = it->second;

  if (freq_stat == NULL) {
    gLog("[%s][%d]: type name %s freq_stat NULL\n",
        __FILE__, __LINE__, type_name);
    return false;
  }


  FreqItem *freq_item = NULL;
  uint32_t& last_time = freq_stat->GetItem(key, appid, 0, freq_item);
  if (freq_item == NULL)  {
    return true;
  }

  if (GetWhiteLevel(freq_item) > 0) {
    return true;
  }

  last_time = 1;
  return true;
}

uint32_t FreqManager::GetWhiteLevel(const FreqItem* item) {
  return item->level1_cnt == kWhiteFlag ? 1 : 0;
}

uint32_t FreqManager::GetBlackLevel(const FreqItem* item) {
  if (item->level1_cnt == kAddBlockFlag) {
    return item->level2_cnt;
  }
  return 0;
}

}  // namespace libwxfreq
