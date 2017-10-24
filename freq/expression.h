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

#ifndef FREQLIB_FREQ_EXPRESSION_H_
#define FREQLIB_FREQ_EXPRESSION_H_
#include <string>
#include "freq_item.h"

namespace libwxfreq {

class Expression {
 public:
  virtual bool ParseItem(const std::string& item) = 0;
  virtual bool IsMatch(const FreqItem& freqitem) const = 0;
  virtual std::string DebugString() const = 0;
  virtual Expression* Clone() const = 0;
};


}  // namespace libwxfreq


#endif  // FREQLIB_FREQ_EXPRESSION_H_
