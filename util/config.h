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

#ifndef FREQLIB_UTIL_CONFIG_H_
#define FREQLIB_UTIL_CONFIG_H_

#include <string>
#include <vector>
#include <sstream>
#include "config_hash.h"

namespace libwxfreq {

class Config {
 public:
  Config() { }
  ~Config() { }
  explicit Config(const std::string &configfile);

  int Init();
  const std::string & conf_filename();
  void set_conf_filename(const std::string &conf_filename);

  void section_list(std::vector<std::string>& sectionlist);
  void GetKeysBySection(const std::string &section,
                        std::vector<std::string>& keys) const;

  template <typename T1, typename T2>
  int ReadItem(const std::string& section, const std::string& key,
              const T1& defaultvalue, T2& itemvalue) const;

  int ReadItem(const std::string& section, const std::string& key,
              const char* defaultvalue, std::string& itemvalue) const;

 private:
  int Reset();
  int ParserConfig();
  int LoadFile(void);

  std::string conf_filename_;
  std::string conf_filename_buf_;
  std::vector<Slice> section_list_;
  ConfigHashTable table_;
};

template <typename T1, typename T2>
int Config::ReadItem(const std::string& section, const std::string& key,
                    const T1& defaultvalue, T2& itemvalue) const {
  if (section.size() <= 0 || key.size() <= 0 ) return -1;

  std::string value;
  Slice idxsec = section;
  Slice idxkey = key;
  Slice val = table_.Get(idxsec, idxkey);

  if (val.start() != NULL) {
    value.assign(val.start(), val.end() - val.start());
    std::stringstream ss(value, std::ios_base::in);
    ss >> itemvalue;
    return 0;
  } else {
    std::stringstream ss;
    ss << defaultvalue;
    ss >> itemvalue;
    return 1;
  }
}
}  // namespace libwxfreq

#endif  // FREQLIB_UTIL_CONFIG_H_
