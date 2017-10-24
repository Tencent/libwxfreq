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

#include "libwxfreq/libwxfreq.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[]) {
  if (argc != 7){
    printf("%s conf appid key keytype shm_key sleep_time(ms)\n", argv[0]);
    return 0;
  }
  SetRuleConfFile(argv[1]); 
  int appid = strtoul(argv[2], NULL, 10);
  const char* key = argv[3];
  const char* keytype = argv[4];
  int shm_key = strtoul(argv[5], NULL, 10);
  int sleep_time = strtoul(argv[6], NULL, 10);
  sleep_time *= 1000;
  
  int ret = RegisterNewShmStat(keytype, false, shm_key,  1000000);
  if (ret != 0) {
    printf("RegisterNewShmStat failed, ret = %d\n", ret);
    return -1;
  }

  if (InitFreq() == false) {
    printf("InitFreq failed\n");
    return -2;
  }

  while (true) {
    struct BlockResult res;
    res = ReportAndCheck(keytype, key, appid, 1);
    printf("time %lu blocklevel %u matchrule %s\n", time(NULL),
          res.block_level, res.match_rule);
    usleep(sleep_time);
  }
}
