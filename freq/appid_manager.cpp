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

#include "appid_manager.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "config.h"
#include "log.h"


namespace libwxfreq {

static AppidMeta  dummymeta;
AppidMapVecAppidMeta  AppidManager::appid_map_[2];
unsigned char AppidManager::index_ = 0;

void AppidManager::UpdateAppidMeta(const Config& config) {
  int ret = 0;
  unsigned int appid = 0; 
  std::string interval_value; //60, 3600, 86400

  unsigned char new_index = (index_ + 1) % 2;
  AppidMapVecAppidMeta& tmp_appid  = appid_map_[new_index];
  tmp_appid.clear();

  std::vector<std::string> keys;
  config.GetKeysBySection("appid", keys);

  for (std::vector<std::string>::iterator it = keys.begin();
      it != keys.end(); ++it) {
    appid = atoi(it->c_str());
    ret = config.ReadItem("appid", *it, "", interval_value);
    if (ret != 0 || appid == 0) {
      gLog("[%s][%d]: read %s appid %d failed %d\n", __FILE__, __LINE__,
          it->c_str(), appid, ret);
      continue;
    }

    int32_t min = 0, mid = 0, max = 0;
    char *save_ptr = NULL;
    char *p = NULL;
    char buf[128];
    snprintf(buf, sizeof(buf), "%s", interval_value.c_str());
    p = strtok_r(buf, ",", &save_ptr);
    if (p != NULL) {
      min = atoi(p);
      if (min > 0 && min <= 86400) {
        tmp_appid[appid].set_min_interval(min);
      }
    }

    p = strtok_r(NULL, ",", &save_ptr);
    if (p != NULL) {
      mid = atoi(p);
      if (mid > 0 && mid <= 86400) {
        tmp_appid[appid].set_mid_interval(mid);
      }
    }

    p = strtok_r(NULL, ",", &save_ptr);
    if (p != NULL) {
      max = atoi(p);
      if (max > 0 && max <= 86400) {
        tmp_appid[appid].set_max_interval(max);
      }
    }
  }

  index_ = new_index;
  gLog("[%s][%d]: index = %d\n", __FILE__, __LINE__, index_);
  for (AppidMapVecAppidMeta::iterator it = appid_map_[index_].begin();
      it != appid_map_[index_].end(); ++it) {
    gLog("[%s][%d]: appid = %u, meta = %s\n", __FILE__, __LINE__,
        it->first, it->second.DebugString().c_str());
  }
}

const AppidMeta* AppidManager::GetAppidMeta(const uint32_t appid) {
  AppidMapVecAppidMeta::iterator it = appid_map_[index_].find(appid);
  if (it == appid_map_[index_].end()) {
    gLog("[%s][%d]: can't find appid %d meta\n", __FILE__, __LINE__, appid);
    return NULL;
  }
  return &(it->second);
}

}  // idspace libwxfreq
