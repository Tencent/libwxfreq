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

#include "plain_expression.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "slice.h"
#include "log.h"

namespace libwxfreq {

static bool Greater(const FreqItem& freqitem, const unsigned int index,
                    const unsigned int threshold) {
  if (index >= 3) return false;
  const unsigned int *p = (const unsigned int *)&freqitem;
  return p[index] > threshold;
}

static bool GreaterOrEqual(const FreqItem& freqitem, const unsigned int index,
                          const unsigned int threshold) {
  if (index >= 3) return false;
  const unsigned int *p = (const unsigned int *)&freqitem;
  return p[index] >= threshold;
}

bool PlainExpression::ParseItem(const std::string& item) {
  const char *ptr = NULL;
  ptr = strstr(item.c_str(), ">");
  if (ptr == NULL) return false;
  Slice keyword(item.c_str(), ptr);
  keyword.StrTrim(" ");
  if (keyword == "min_interval") {
    key_index_ = 0;
  } else if (keyword == "mid_interval") {
    key_index_ = 1;
  } else if (keyword == "max_interval") {
    key_index_ = 2;
  } else {
    gLog("[%s][%d]: wrong keyword %s, item %s\n",
        __FILE__, __LINE__, keyword.ToStr().c_str(), item.c_str());
    return false;
  }
  ptr++;
  if (*ptr == '=') {
    ptr++;
    cmp_ = GreaterOrEqual;
  } else {
    cmp_ = Greater;
  }

  if (*ptr == '\0') {
    gLog("[%s][%d]: wrong syntax item %s\n", __FILE__, __LINE__, item.c_str());
    return false;
  }
  threshold_ = strtoul(ptr, NULL, 10);
  return threshold_ > 0;
}

bool PlainExpression::IsMatch(const FreqItem& freqitem) const {
    return cmp_(freqitem, key_index_, threshold_);
}

std::string PlainExpression::DebugString() const {
  std::string op = "NULL";
  if (cmp_ == GreaterOrEqual) op = "GreaterOrEqual";
  else if (cmp_ == Greater) op = "Greater";

  char buf[128];
  snprintf(buf, sizeof(buf), "key_index:%u\tthreshold:%u\tcmp:%s\taddr:%lx",
          key_index_, threshold_, op.c_str(), (unsigned long int)cmp_);

  return std::string(buf);
}

Expression* PlainExpression::Clone() const {
  return new PlainExpression(cmp_, threshold_, key_index_);
}

}  // namespace libwxfreq
