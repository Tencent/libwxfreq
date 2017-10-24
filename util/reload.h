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

#ifndef FREQLIB_UTIL_RELOAD_H_
#define FREQLIB_UTIL_RELOAD_H_
#include <stdint.h>
namespace libwxfreq {

typedef int (*LoadFunc)(const char* type_name);
typedef int (*DumpFunc)(const char* opname, const char* type_name,
                        const char *key, const uint32_t appid,
                        const uint32_t linger_time,
                        const uint32_t block_level);

extern LoadFunc gLoadFunc;
extern DumpFunc gDumpFunc;

}  // namespace libwxfreq
#endif  // FREQLIB_UTIL_RELOAD_H_
