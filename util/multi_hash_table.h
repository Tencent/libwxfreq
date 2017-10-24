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

#ifndef FREQLIB_UTIL_MULTI_HASH_TABLE_H_
#define FREQLIB_UTIL_MULTI_HASH_TABLE_H_

#include <sys/shm.h>
#include <sys/stat.h>
#include "multi_hash_base.h"
#include "freq_item.h"

namespace libwxfreq {

#pragma pack(1)
struct FreqItemForShm:public libwxfreq::FreqItem {
  uint32_t timestamp;
};
#pragma pack()

class MultiHashTable : public MultiHashBase<libwxfreq::FreqKey, FreqItemForShm> {
 public:
  MultiHashTable();
  virtual ~MultiHashTable();

 public:
  int MultiHashTableInit(bool zero_init, key_t shmkey, int shmflag,
                        uint64_t item_cnt);
  bool IsValid();
  void MarkAsValid();
  void MarkAsInValid();

 protected:
  size_t HashKey(const KeyType &input_key);
  bool KeyCmp(const KeyType &input_key, const KeyType &key_in_mem);
  bool IsExpired(const KeyType &input_key, const ValueType &value,
                void *old_value);
  bool IsEmpty(const KeyType &input_key, const ValueType &value);
  void AfterFindReadItem(ValueType &value);
  void AfterFindWriteItem(ValueType &value);
  bool ExpiredOccupy(const KeyType &input_key, KeyType &key_in_mem,
                    ValueType &value_in_mem, void *old_value);
  bool EmptyOccupy(const KeyType &input_key, KeyType &key_in_mem);

 private:
  char * init_flag_;
};

}  // namespace libwxfreq
#endif  // FREQLIB_UTIL_MULTI_HASH_TABLE_H_
