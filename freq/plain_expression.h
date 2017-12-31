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

#ifndef FREQLIB_FREQ_PLAIN_EXPRESSION_H_
#define FREQLIB_FREQ_PLAIN_EXPRESSION_H_

#include <string>
#include "expression.h"
#include "log.h"

namespace libwxfreq {

class PlainExpression: public Expression {
 public:
  PlainExpression() { }

  bool ParseItem(const std::string& item);
  bool IsMatch(const FreqItem& freqitem) const;
  std::string DebugString() const;
  Expression* Clone() const;

 private:
  typedef bool (*cmp)(const FreqItem&, const unsigned int, const unsigned int);

  PlainExpression(const cmp func, const unsigned int threshold,
                  const unsigned char key_index)
    : cmp_(func),
      threshold_(threshold),
      key_index_(key_index) { }

  // parse item
  cmp cmp_;
  unsigned int threshold_;
  uint8_t key_index_;
};

}  // namespace libwxfreq

#endif  // FREQLIB_FREQ_PLAIN_EXPRESSION_H_
