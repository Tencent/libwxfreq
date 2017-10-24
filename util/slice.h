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

#ifndef FREQLIB_UTIL_SLICE_H_
#define FREQLIB_UTIL_SLICE_H_

#include<string>

namespace libwxfreq {

class Slice {
 public:
  Slice();
  Slice(const char *start, const char *end);
  Slice(const std::string &other_str);

  std::string ToStr() const;
  void StrTrim(const char *delimiter);

  inline const char *start() const { return start_; }
  inline const char *end() const { return end_; }

  bool operator!=(const Slice &other_slice) const;
  bool operator==(const Slice &other_slice) const;
  bool operator==(const std::string &other_str) const;
  bool operator<(const Slice &other_slice) const;

 private:
  const char *start_, *end_;
};


}  //  namespace libwxfreq
#endif  // FREQLIB_UTIL_SLICE_H_
