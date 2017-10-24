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

#include "libwxfreq.h"
#include <string.h>
#include <stdio.h>
#include "appid_manager.h"
#include "freq_manager.h"
#include "rule_manager.h"
#include "map_freq_stat.h"
#include "shm_freq_stat.h"
#include "options.h"
#include "freq_stat.h"
#include "log.h"
#include "reload.h"

using namespace libwxfreq;

const static uint32_t kDeleteTime = 86400;
TypeName2FreqStatMap type_name_map;

int RegisterNewMapStat(const char* type_name, bool /*zero_init*/,
                      key_t /*key*/, unsigned int /*item_cnt*/) {
  if (type_name == NULL) {
    gLog("[%s][%d]: type name empty\n", __FILE__, __LINE__);
    return kErrorInput;
  }
  TypeName2FreqStatMap::iterator it = type_name_map.find(type_name);
  if (it != type_name_map.end()) delete it->second;
  FreqStat * tmp_map = new  MapFreqStat();
  if (tmp_map == NULL) {
    gLog("[%s][%d]: alloc stat memory error, type name:%s\n",
        __FILE__, __LINE__, type_name);
    return kErrorSystem;
  }
  tmp_map->set_type_name(type_name);
  type_name_map[type_name] = tmp_map;
  return kOK;
}

int RegisterNewShmStat(const char* type_name, bool zero_init,
                      key_t key, unsigned int item_cnt) {
  if (type_name == NULL) {
    gLog("[%s][%d]: type name empty\n", __FILE__, __LINE__);
    return kErrorInput;
  }
  TypeName2FreqStatMap::iterator it = type_name_map.find(type_name);
  if (it != type_name_map.end()) delete it->second;
  FreqStat * tmp_map = new  ShmFreqStat(zero_init, key, item_cnt);
  if (tmp_map == NULL) {
    gLog("[%s][%d]: alloc stat memory error, type name:%s\n",
        __FILE__, __LINE__, type_name);
    return kErrorSystem;
  }
  tmp_map->set_type_name(type_name);
  type_name_map[type_name] = tmp_map;
  return kOK;
}

void SetRuleConfFile(const char *filename) {
  RuleManager::set_rule_conf_file(filename);
}

bool InitFreq() {
  RuleManager::Init();
  return FreqManager::Init(type_name_map);
}

static BlockResult MakeBlockResult(unsigned int block_level,
                                  const char* match_rule) {
  BlockResult result;
  result.block_level = block_level;
  strncpy(result.match_rule, match_rule, sizeof(result.match_rule));
  return result;
}

BlockResult ReportAndCheck(const char* type_name, const char *key,
                          const uint32_t appid, const uint32_t cnt) {
  const FreqItem* p =  NULL;
  p = FreqManager::Add(type_name, key, appid, cnt);
  if (p == NULL) return MakeBlockResult(kErrorOutOfMemory, "");

  RuleManager::Result res = RuleManager::IsMatchRule(type_name, appid, *p);
  return MakeBlockResult(res.block_level, res.match_rule.c_str());
}

struct BlockResult OnlyCheck(const char* type_name, const char *key,
                            const uint32_t appid) {
  return ReportAndCheck(type_name, key, appid, 0);
}

int OnlyReport(const char* type_name, const char *key, const uint32_t appid,
              const uint32_t cnt) {
  if (FreqManager::Add(type_name, key, appid, cnt) == NULL)
    return kErrorOutOfMemory;
  return 0;
}


static FreqCache MakeFreqCache(const uint32_t level1, const uint32_t level2,
                              const uint32_t level3) {
  FreqCache freqcache = {level1, level2, level3};
  return freqcache;
}


FreqCache GetCache(const char* type_name,  const char *key,
                  const uint32_t appid, const uint32_t cnt) {
  const FreqItem* p =  NULL;
  p = FreqManager::Add(type_name, key, appid, cnt);
  if (p == NULL) return MakeFreqCache(0, 0, 0);
  return MakeFreqCache(p->level1_cnt, p->level2_cnt, p->level3_cnt);
}

void SetLogFunc(LogFunction logfunc) {
  SetLog(logfunc);
}

int AddWhite(const char* type_name, const char *key, const uint32_t appid,
            const uint32_t linger_time) {
  if (gDumpFunc("AddWhite", type_name, key, appid, linger_time, 1) != 0)
    return kErrorSystem;
  if (FreqManager::AddWhite(type_name, key, appid, linger_time) == false)
    return kErrorOutOfMemory;
  return 0;
}

int DeleteWhite(const char* type_name, const char *key, const uint32_t appid) {
  if (gDumpFunc("DeleteWhite", type_name, key, appid, kDeleteTime, 1) != 0)
    return kErrorSystem;
  if (FreqManager::DeleteWhite(type_name, key, appid) == false)
    return kErrorOutOfMemory;
  return 0;
}

int AddBlock(const char* type_name, const char *key, const uint32_t appid,
            const uint32_t linger_time, const uint32_t block_level) {
  if (gDumpFunc("AddBlock", type_name, key, appid,
                linger_time, block_level) != 0) return kErrorSystem;
  if (FreqManager::AddBlock(type_name, key, appid, linger_time,
                            block_level) == false) return kErrorOutOfMemory;
  return 0;
}

int DeleteBlock(const char* type_name, const char *key, const uint32_t appid) {
  if (gDumpFunc("DeleteBlock", type_name, key, appid, kDeleteTime, 1) != 0)
    return kErrorSystem;
  if (FreqManager::DeleteBlock(type_name, key, appid) == false)
    return kErrorOutOfMemory;
  return 0;
}

void SetLoadFunc(LoadFunc func) {
  libwxfreq::SetLoadFunc(func);
}

void SetDumpFunc(DumpFunc func) {
  libwxfreq::SetDumpFunc(func);
}
