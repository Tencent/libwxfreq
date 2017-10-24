#include <stdlib.h>
#include "config_hash.h"

namespace libwxfreq {

// ConfigHashNode
ConfigHashNode::ConfigHashNode():hash_num_(0) { }

ConfigHashNode::ConfigHashNode(const unsigned int hash_num, const Slice &sec,
                 const Slice &key, const Slice &val) {
  hash_num_ = hash_num;
  section_ = sec;
  key_ = key;
  value_ = val;
}

inline unsigned ConfigHashNode::hash_num() const {
  return hash_num_;
}

inline const Slice& ConfigHashNode::section() const {
  return section_;
}

inline const Slice& ConfigHashNode::key() const {
  return key_;
}

inline const Slice & ConfigHashNode::value() const {
  return value_;
}

// ConfigHashTable
ConfigHashTable::ConfigHashTable() {
}

ConfigHashTable::ConfigHashTable(const ConfigHashTable &other) {
  table_ = other.table_;
}

ConfigHashTable::~ConfigHashTable() {
}

void ConfigHashTable::Clear() {
  table_.clear();
}

// using FNV1 hash function
inline unsigned int ConfigHashTable::HashFun(const Slice &sec,
                                            const Slice &key) const {
  static const unsigned int FNV1_BASIS = 2166136261lu;
  static const unsigned int FNV1_PRIME = 16777619;
  unsigned int ret = FNV1_BASIS;
  const char *ptr, *endptr;

  endptr = sec.end();
  for (ptr = sec.start(); ptr < endptr; ptr++) {
    ret *= FNV1_PRIME;
    ret ^= tolower(*ptr);
  }

  endptr = key.end();
  for (ptr = key.start(); ptr < endptr; ptr++) {
    ret *= FNV1_PRIME;
    ret ^= tolower(*ptr);
  }
  return ret;
}

void ConfigHashTable::Add(const Slice &sec, const Slice &key,
                          const Slice &val) {
  unsigned int hashnum = HashFun(sec, key);
  table_[hashnum].push_back(ConfigHashNode(hashnum, sec, key, val));

  std::vector<ConfigHashNode>::iterator it = table_[hashnum].end() - 1;
  unsigned int sec_hashnum = HashFun(sec, Slice());
  keys_table_[sec_hashnum].push_back(it);
}

Slice ConfigHashTable::Get(const Slice &sec, const Slice &key) const {
  unsigned int hashnum = HashFun(sec, key);

  HashTable::const_iterator tit = table_.find(hashnum);

  if (tit == table_.end()) return Slice();

  for (std::vector<ConfigHashNode>::const_iterator it = tit->second.begin();
      it != tit->second.end(); ++it) {
    if (it->hash_num() == hashnum && it->section() == sec && it->key() == key) {
      return it->value();
    }
  }

  return Slice();
}

void ConfigHashTable::GetKeysBySection(const Slice &sec,
                                      std::vector<Slice> &keys) const {
  unsigned int hashnum = HashFun(sec, Slice());
  HashIteratorTable::const_iterator tit = keys_table_.find(hashnum);

  if (tit == keys_table_.end()) return;

  for (std::vector<std::vector<ConfigHashNode>::iterator>::const_iterator
      it = tit->second.begin(); it != tit->second.end(); ++it) {
    if ((*it)->section() == sec) {
      keys.push_back((*it)->key());
    }
  }
}

bool ConfigHashTable::Empty() {
  return table_.empty();
}

}  // namespace libwxfreq

