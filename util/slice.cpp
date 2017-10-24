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

#include "slice.h"
#include <stdlib.h>
#include <string.h>


using namespace std;

namespace libwxfreq {

Slice::Slice() {
  start_ = end_ = NULL;
}

Slice::Slice(const char *start, const char *end) {
  start_ = start;
  end_ = end;
}

Slice::Slice(const std::string &other_slice) {
  start_ = other_slice.c_str();
  end_ = other_slice.c_str() + other_slice.size();
}

std::string Slice::ToStr() const {
  std::string ret(start_, end_- start_);
  return ret;
}

void Slice::StrTrim(const char *delimiter) {
  while (start_ < end_ && strchr(delimiter, *start_) != 0) start_++;
  while (start_ < end_ && strchr(delimiter, *(end_-1)) != 0) end_--;
}

bool Slice::operator!=(const Slice &other_slice) const {
  return !(*this == other_slice);
}

bool Slice::operator==(const Slice &other_slice) const {
  if (end_ - start_ != other_slice.end_ - other_slice.start_) return false;
  return strncasecmp(start_, other_slice.start_, end_ - start_) == 0;
}

bool Slice::operator==(const std::string &other_slice)  const {
  if (end_ - start_ != static_cast<int>(other_slice.size())) return false;
  return strncasecmp(start_, other_slice.c_str(), end_ - start_) == 0;
}

bool Slice::operator<(const Slice &other_slice) const {
  if (start_ == other_slice.start_) return false;

  int minlen = end_ - start_, res = 0;
  if (other_slice.end_ - other_slice.start_ < minlen )
    minlen = other_slice.end_ - other_slice.start_;
  if ((res = strncasecmp( start_, other_slice.start_, minlen )) < 0)
    return true;
  else if (res > 0)
    return false;
  else
    return end_ - start_ < other_slice.end_ - other_slice.start_;
}

}  //  namespace libwxfreq
