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

#include "appname_manager.h"
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include "config.h"
#include "log.h"


namespace libwxfreq {

static AppNameMeta  dummymeta;
AppanmeMapVecAppnameMeta  AppnameManager::appname_map_[2];
unsigned char AppnameManager::index_ = 0;

void AppnameManager::UpdateAppNameMeta(const Config& config) {
  int ret = 0;
  unsigned int appid = 0, interval = 0;
  std::string appname, interval_key;

  unsigned char new_index = (index_ + 1) % 2;
  AppanmeMapVecAppnameMeta& tmp_appname  = appname_map_[new_index];
  tmp_appname.clear();

  std::vector<std::string> keys;
  config.GetKeysBySection("appname", keys);

  for (std::vector<std::string>::iterator it = keys.begin();
      it != keys.end(); ++it) {
    appid = 0;
    std::size_t pos = it->find(":");
    if (pos == std::string::npos) {
      ret = config.ReadItem("appname", *it, 0, appid);
      if (ret != 0 || appid == 0) {
        gLog("[%s][%d]: read %s appid %d failed\n", __FILE__, __LINE__,
            it->c_str(), appid);
        continue;
      }
      tmp_appname[*it].set_appname(*it);
      tmp_appname[*it].set_appid(appid);
    } else {
      appname = it->substr(0, pos);
      interval_key = it->substr(pos + 1);
      AppNameMeta& meta = tmp_appname[appname];

      ret = config.ReadItem("appname", *it, 0, interval);
      if (ret != 0) {
        gLog("[%s][%d]: read %s %s failed\n", __FILE__, __LINE__,
            appname.c_str(), interval_key.c_str());
        continue;
      }
      if (interval_key == "min_interval") {
        meta.set_min_interval(interval);
      } else if (interval_key == "mid_interval") {
        meta.set_mid_interval(interval);
      } else if (interval_key == "max_interval") {
        meta.set_max_interval(interval);
      }
    }
  }
  index_ = new_index;
  gLog("[%s][%d]: index = %d\n", __FILE__, __LINE__, index_);
  for (AppanmeMapVecAppnameMeta::iterator it = appname_map_[index_].begin();
      it != appname_map_[index_].end(); ++it) {
    gLog("[%s][%d]: appname = %s, meta = %s\n", __FILE__, __LINE__,
        it->first.c_str(), it->second.DebugString().c_str());
  }
}

const AppNameMeta* AppnameManager::GetAppNameMeta(const std::string& appname) {
  AppanmeMapVecAppnameMeta::iterator it = appname_map_[index_].find(appname);
  if (it == appname_map_[index_].end()) {
    gLog("[%s][%d]: can't find appname %s meta\n", __FILE__, __LINE__,
        appname.c_str());
    return NULL;
  }
  return &(it->second);
}

}  // namespace libwxfreq
