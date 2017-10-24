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

#ifndef FREQLIB_FREQ_FREQ_ITEM_H_
#define FREQLIB_FREQ_FREQ_ITEM_H_
#include <stdint.h>
#include <string>

#pragma pack(1)
namespace libwxfreq {

struct FreqKey {
  char key[32];
  uint32_t appid;
};

struct FreqItem {
  unsigned int level1_cnt;
  unsigned int level2_cnt;
  unsigned int level3_cnt;
};

}  // namespace libwxfreq
#pragma pack()
#endif  // FREQLIB_FREQ_FREQ_ITEM_H_
