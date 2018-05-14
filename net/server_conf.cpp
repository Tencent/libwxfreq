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

#include "server_conf.h"
#include "config.h"

namespace libwxfreq {
using std::string;
using std::to_string;

StatType::StatType(const StatType& left):typename_(left.typename_),
    init_(left.init_), shm_key_(left.shm_key_), item_count_(left.item_count_) {
}

StatType::StatType(const std::string& tyname, uint32_t init,
              uint32_t shm_key, uint32_t item_count)
              :typename_(tyname), init_(init), shm_key_(shm_key),
              item_count_(item_count) {
}

const std::string& StatType::GetTypeName() const {
  return typename_;
}
uint32_t StatType::init() const {
  return init_;
}
uint32_t StatType::shm_key() const {
  return shm_key_;
}

uint32_t StatType::item_count() const {
  return item_count_;
}

ServerConf& ServerConf::GetInstance() {
    static ServerConf conf_;
    return conf_;
}

ServerConf::ServerConf() {
  has_init_ = false;
}

bool ServerConf::LoadFile(const std::string& file) {
  if (has_init_) return true;

  Config config(file);
  if (config.Init() != 0) return false;

  int32_t stat_num = 0;
  config.ReadItem("General", "libwxfreq_conf_path", "", libwxfreq_conf_path_);
  config.ReadItem("General", "stat_type_num", 0, stat_num);

  config.ReadItem("Server", "listen_ip", "", listen_ip_);
  config.ReadItem("Server", "listen_port", 0, listen_port_);
  config.ReadItem("Server", "worke_num", 0, worke_num_);
  config.ReadItem("Server", "timeout", 1, timeout_);
  stat_vec_.clear();
  for (int i = 0; i < stat_num; i++) {
    string typename_, section_name = "Stat_";
    uint32_t init = 0, shm_key = 0, item_count;
    section_name += to_string(i);

    config.ReadItem(section_name, "typename", "", typename_);
    config.ReadItem(section_name, "init", 0, init);
    config.ReadItem(section_name, "shm_key", 0, shm_key);
    config.ReadItem(section_name, "item_count", 0, item_count);

    StatType type(typename_, init, shm_key, item_count);
    stat_vec_.push_back(type);
  }

  if (stat_num > 0) {
    has_init_ = true;
  }
  return has_init_;
}

const std::string ServerConf::listen_ip() const {
  return listen_ip_;
}
const std::string ServerConf::libwxfreq_conf_path() const {
  return libwxfreq_conf_path_;
}
uint32_t ServerConf::listen_port() const {
  return listen_port_;
}
uint32_t ServerConf::worker_num() const {
  return worke_num_;
}
const std::vector<StatType>& ServerConf::stat_vec() const {
  return stat_vec_;
}
uint32_t ServerConf::timeout() const {
  return timeout_;
}

}  // namespace libwxfreq

