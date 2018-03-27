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

#ifndef LIBFREQ_NET_SERVER_CONF_H_
#define LIBFREQ_NET_SERVER_CONF_H_
#include <stdint.h>
#include <string>
#include <vector>


namespace libwxfreq {
class StatType {
 public:
  StatType(const StatType& left);
  StatType(const std::string& tyname, uint32_t init,
              uint32_t shm_key, uint32_t item_count);
  const std::string& GetTypeName() const;
  uint32_t init() const;
  uint32_t shm_key() const;
  uint32_t item_count() const;

 private:
  std::string typename_;
  uint32_t init_;
  uint32_t shm_key_;
  uint32_t item_count_;
};

class ServerConf {
 public:
  static ServerConf& GetInstance();
  ServerConf(const ServerConf& a) = delete;
  bool LoadFile(const std::string& file);
  const std::string listen_ip() const;
  const std::string libwxfreq_conf_path() const;
  uint32_t listen_port() const;
  uint32_t worker_num() const;
  const std::vector<StatType>& stat_vec() const;
  uint32_t timeout() const;

 private:
  ServerConf();

 private:
  std::string libwxfreq_conf_path_;
  uint32_t stat_type_num_;
  std::string listen_ip_;
  uint32_t listen_port_;
  uint32_t worke_num_;
  uint32_t timeout_;
  std::vector<StatType> stat_vec_;
  bool has_init_;
};

}  // namespace libwxfreq
#endif  // LIBFREQ_NET_SERVER_CONF_H_
