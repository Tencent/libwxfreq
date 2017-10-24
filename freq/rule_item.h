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

#ifndef FREQLIB_FREQ_RULE_ITEM_H_
#define FREQLIB_FREQ_RULE_ITEM_H_

#include <stdio.h>
#include <string>
#include "expression.h"

namespace libwxfreq {
class FreqItem;

class RuleItem {
 public:
  RuleItem(const std::string& rulename, uint16_t blocklevel)
    : match_rulename_(rulename),
    block_level_(blocklevel),
    expression_(NULL) { }

  RuleItem(const RuleItem& other) {
    *this = other;
    expression_ = other.expression_->Clone();
  }

  ~RuleItem() {
    if (expression_ != NULL) delete expression_;
  }

  inline uint16_t block_level() const {
    return block_level_;
  }

  inline const std::string& match_rulename() const {
    return match_rulename_;
  }

  inline void set_match_rulename(const std::string& match_rulename) {
    match_rulename_ = match_rulename;
  }

  inline void set_block_level(uint16_t block_level) {
    block_level_ = block_level;
  }

  inline void set_expression(Expression* p) {
    if (expression_ != NULL) delete expression_;
    expression_ = p;
  }

  inline Expression* expression() {
    return expression_;
  }

  inline bool ParseItem(const std::string& item) {
    return expression_->ParseItem(item);
  }

  inline bool IsMatch(const FreqItem& freqitem) const {
    return expression_->IsMatch(freqitem);
  }

  inline std::string DebugString() {
    std::string expression = expression_->DebugString();
    char buf[512];
    snprintf(buf, sizeof(buf), "match_rulename:%s\tblock_level:%u\t"
            "expression:%s", match_rulename_.c_str(), block_level_,
            expression.c_str());
    return std::string(buf);
  }

 private:
  std::string match_rulename_;
  uint16_t block_level_;
  Expression* expression_;
};

}  // namespace libwxfreq
#endif  // FREQLIB_FREQ_RULE_ITEM_H_
