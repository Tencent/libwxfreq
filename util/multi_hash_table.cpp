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

#include "multi_hash_table.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
namespace libwxfreq {

static size_t BKDRHash(const char *str) {
  unsigned int seed = 131;  // 31 131 1313 13131 131313 etc..
  unsigned int hash = 0;
  while (*str) {
    hash = hash * seed + (*str++);
  }
  return hash;
}

static size_t IntHash(uint32_t appid) {
  register size_t key = appid;
  return key * 2654435761;
}

MultiHashTable::MultiHashTable():init_flag_(NULL) { }
MultiHashTable::~MultiHashTable() { }

int MultiHashTable::MultiHashTableInit(bool zero_init, key_t shmkey,
                                       int shmflag, uint64_t item_cnt) {
  size_t shm_size = (size_t)(item_size() * item_cnt + 1);

  int iShmID = 0;
  iShmID = ::shmget(shmkey, shm_size, shmflag | IPC_CREAT);
  if (iShmID < 0 && errno == EINVAL) {
    // 共享内存大小发生变化
    iShmID = ::shmget(shmkey, 0, 0);
    if (iShmID == -1) return -__LINE__;
    if (::shmctl(iShmID, IPC_RMID, NULL) == -1) return -__LINE__;
    iShmID = ::shmget(shmkey, shm_size, shmflag | IPC_CREAT);
  }
  if (iShmID <0) return -__LINE__;

  char *base = NULL;
  base = reinterpret_cast<char *>(::shmat(iShmID, NULL, 0));
  if (reinterpret_cast<char *>(-1) == base) {
    return -__LINE__;
  }

  init_flag_ = base + shm_size - 1;
  if (zero_init) {
    memset(base, 0, shm_size);
  }

  return MultiHashInit(base, shm_size);
}

size_t MultiHashTable::HashKey(const KeyType &input_key) {
  return BKDRHash(input_key.key) * IntHash(input_key.appid);
}

bool MultiHashTable::KeyCmp(const KeyType &input_key,
                            const KeyType &key_in_mem) {
  size_t size = sizeof(input_key.key);
  uint32_t input_hash = *reinterpret_cast<const uint32_t*>(
      input_key.key + size - sizeof(uint32_t));
  uint32_t mem_hash = *reinterpret_cast<const uint32_t*>(
      key_in_mem.key + size - sizeof(uint32_t));
  return input_key.appid == key_in_mem.appid &&
         input_hash == mem_hash &&
         strncmp(key_in_mem.key, input_key.key, size) == 0;
}

bool MultiHashTable::IsExpired(const KeyType &input_key,
                              const ValueType &value, void *old_value) {
  *reinterpret_cast<uint32_t*>(old_value)  = value.timestamp;
  return value.timestamp != 0 && time(NULL) - value.timestamp >= 90000;
}

bool MultiHashTable::IsEmpty(const KeyType &input_key,
                            const ValueType &/*value*/) {
  return input_key.appid == 0;
}
void MultiHashTable::AfterFindReadItem(ValueType &value) { }
void MultiHashTable::AfterFindWriteItem(ValueType &value) { }

bool MultiHashTable::ExpiredOccupy(const KeyType &input_key,
                                  KeyType &key_in_mem, ValueType &value_in_mem,
                                  void *old_value) {
  if (__sync_bool_compare_and_swap(&value_in_mem.timestamp,
                                  *reinterpret_cast<uint32_t*>(old_value), 0)) {
    memcpy(key_in_mem.key, input_key.key, sizeof(input_key.key));
    key_in_mem.appid = input_key.appid;
    return true;
  }
  return false;
}

bool MultiHashTable::EmptyOccupy(const KeyType &input_key,
                                KeyType &key_in_mem) {
  if (__sync_bool_compare_and_swap(&key_in_mem.appid, 0, input_key.appid)) {
    memcpy(key_in_mem.key, input_key.key, sizeof(input_key.key));
    return true;
  }
  return false;
}

bool MultiHashTable::IsValid() {
  return init_flag_!= NULL && *init_flag_ == 17;
}

void MultiHashTable::MarkAsValid() {
  if (init_flag_ != NULL) *init_flag_ = 17;
}

void MultiHashTable::MarkAsInValid() {
  if (init_flag_ != NULL) *init_flag_ = 0;
}

}  //  namespace libwxfreq

