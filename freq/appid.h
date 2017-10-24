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

#ifndef FREQLIB_FREQ_APPID_H_
#define FREQLIB_FREQ_APPID_H_

#include <stdint.h>
#include <stdio.h>
#include <string>

namespace libwxfreq {

class AppidMeta {
 public:
  AppidMeta()
    : min_interval_(60),
      mid_interval_(3600),
      max_interval_(86400) {
  }

  inline void set_min_interval(const uint32_t& min_interval) {
    min_interval_ = min_interval;
  }

  inline void set_mid_interval(const uint32_t& mid_interval) {
    mid_interval_ = mid_interval;
  }

  inline void set_max_interval(const uint32_t& max_interval) {
    max_interval_ = max_interval;
  }

  inline uint16_t min_interval() const {
    return min_interval_;
  }

  inline uint16_t mid_interval() const {
    return mid_interval_;
  }
  inline uint16_t max_interval() const {
    return max_interval_;
  }

  inline std::string DebugString() const {
    char buf[128];
    snprintf(buf, sizeof(buf), "min_interval=%d, mid_interval=%d,"
            " max_interval=%d", min_interval_, mid_interval_, max_interval_);
    return buf;
  }

 private:
  uint16_t min_interval_;
  uint16_t mid_interval_;
  uint32_t max_interval_;
};

}  // namespace libwxfreq

#endif  // FREQLIB_FREQ_APPID_H_
