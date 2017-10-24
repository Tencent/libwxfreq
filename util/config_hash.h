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
