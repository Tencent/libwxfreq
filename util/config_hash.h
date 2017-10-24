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

#ifndef FREQLIB_UTIL_CONFIG_HASH_H_
#define FREQLIB_UTIL_CONFIG_HASH_H_

#include<vector>
#include<string>
#include<map>
#include "slice.h"

namespace libwxfreq {

class ConfigHashNode {
 public:
  ConfigHashNode();
  ConfigHashNode(const unsigned int hash_num, const Slice &sec,
                 const Slice &key, const Slice &val);
  unsigned hash_num() const;
  const Slice & section() const;
  const Slice & key() const;
  const Slice & value() const;

 private:
  unsigned int hash_num_;
  Slice section_, key_, value_;
};



class ConfigHashTable {
 public:
  ConfigHashTable();
  ConfigHashTable(const ConfigHashTable &other);
  ~ConfigHashTable();


  void Clear();
  void Add(const Slice &sec, const Slice &key, const Slice &val);
  Slice Get(const Slice &sec, const Slice &key) const;
  void GetKeysBySection(const Slice &sec, std::vector<Slice> &keys) const;
  bool Empty();

 private:
  typedef std::map<unsigned int, std::vector<ConfigHashNode> > HashTable;
  typedef std::map<unsigned int, std::vector<std::vector<ConfigHashNode>::iterator> > HashIteratorTable;
  // using FNV1 hash function
  inline unsigned int HashFun(const Slice &sec, const Slice &key) const;
  HashTable table_;
  HashIteratorTable keys_table_;
};

}  // namespace libwxfreq

#endif  // FREQLIB_UTIL_CONFIG_HASH_H_
