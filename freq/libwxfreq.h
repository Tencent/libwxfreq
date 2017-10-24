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

#ifndef FREQLIB_FREQ_FREQLIB_H_
#define FREQLIB_FREQ_FREQLIB_H_
#include <sys/shm.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

enum {
  kOK = 0,
  kErrorOutOfMemory = -1,
  kErrorNoAppName = -2,
  kErrorSystem = -3,
  kErrorInput = -4,
};

struct BlockResult {
    int block_level;
    char match_rule[32];
};

struct FreqCache {
  uint32_t level1_cnt;
  uint32_t level2_cnt;
  uint32_t level3_cnt;
};

int RegisterNewShmStat(const char* type_name, bool zero_init, key_t key,
                      unsigned int item_cnt);
int RegisterNewMapStat(const char* type_name, bool zero_init, key_t key,
                      unsigned int item_cnt);

void SetRuleConfFile(const char *filename);

bool InitFreq();

struct BlockResult ReportAndCheck(const char* type_name, const char *key,
                                  const uint32_t appid, const uint32_t cnt);
struct BlockResult OnlyCheck(const char* type_name, const char *key,
                            const uint32_t appid);
int OnlyReport(const char* type_name, const char *key, const uint32_t appid,
              const uint32_t cnt);
struct FreqCache GetCache(const char* type_name, const char *key,
                          const uint32_t appid, const uint32_t cnt = 0);

int AddWhite(const char* type_name, const char *key, const uint32_t appid,
            const uint32_t linger_time);
int DeleteWhite(const char* type_name, const char *key, const uint32_t appid);
int AddBlock(const char* type_name, const char *key, const uint32_t appid,
            const uint32_t linger_time, const uint32_t block_level);
int DeleteBlock(const char* type_name, const char *key, const uint32_t appid);

typedef int (*LogFunction)(const char* format, ...);
void SetLogFunc(LogFunction logfunc);

typedef int (*LoadFunc)(const char* type_name);
void SetLoadFunc(LoadFunc func);

typedef int (*DumpFunc)(const char* opname, const char* type_name,
                        const char *key, const uint32_t appid,
                        const unsigned int linger_time,
                        const uint32_t block_level);
void SetDumpFunc(DumpFunc func);

#ifdef __cplusplus
}
#endif

#endif  // FREQLIB_FREQ_FREQLIB_H_
