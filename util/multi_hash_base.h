#ifndef FREQLIB_UTIL_MULTI_HASH_BASE_H_
#define FREQLIB_UTIL_MULTI_HASH_BASE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "log.h"

namespace libwxfreq {

static const unsigned int   MAX_LEVEL_CNT       = 30;
static const unsigned int   MAIN_LEVEL_CNT      = 5;
static const float    MAIN_LEVEL_ITEM_RATE  = 0.9;


template <typename Key, typename Value>
class MultiHashBase {

 enum MultiHashRet {
  enMultiHashKeyNotFound    = -3,
  enMultiHashBucketFull   = -2,
  enMultiHashNotInit      = -1,
  enMultiHashOK         = 0,
  enMultiHashFoundEmptyItem   = 1,
  enMultiHashFoundExpiredItem = 2,
};

 public:
  MultiHashBase();
  virtual ~MultiHashBase();
  typedef Key KeyType;
  typedef Value ValueType;

 public:
  int MultiHashInit(char *base, size_t len,
                    unsigned int max_level = MAX_LEVEL_CNT,
                    unsigned int main_level = MAIN_LEVEL_CNT);

  int GetValueForRead(const KeyType &input_key, ValueType *&value);
  int GetValueForWrite(const KeyType &input_key, ValueType *&value);

 protected:
  virtual int InitLevelHash();

  unsigned int item_size() { return item_size_ ;}
  virtual size_t HashKey(const KeyType &input_key) = 0;
  virtual bool KeyCmp(const KeyType &input_key, const KeyType &key_in_mem) = 0;
  virtual bool IsExpired(const KeyType &key, const ValueType &value,
                        void *old_value) = 0;
  virtual bool ExpiredOccupy(const KeyType &input_key, KeyType &key_in_mem,
                            ValueType &value_in_mem, void *old_value) = 0;
  virtual bool IsEmpty(const KeyType &key, const ValueType &value) = 0;
  virtual bool EmptyOccupy(const KeyType &input_key, KeyType &key_in_mem) = 0;
  virtual void AfterFindReadItem(ValueType &value) = 0;
  virtual void AfterFindWriteItem(ValueType &value) = 0;

 private:
  char  *shm_mem_base_;
  unsigned int  max_level_;
  unsigned int  main_level_;
  unsigned int  item_size_;
  size_t shm_mem_len_;
  size_t max_item_cnt_;
  std::vector<size_t> size_meta_;

 private:
  static size_t GetBiggestPrimer(size_t ulluppervalue);
  static bool IsPrimer(size_t ullvalue);
};


template <typename Key, typename Value>
MultiHashBase<Key, Value>::MultiHashBase()
    : shm_mem_base_(NULL),
      shm_mem_len_(0),
      max_level_(MAX_LEVEL_CNT),
      main_level_(MAIN_LEVEL_CNT) {
  item_size_ = sizeof(KeyType) + sizeof(ValueType);
}

template <typename Key, typename Value>
MultiHashBase<Key, Value>::~MultiHashBase() { }


template <typename Key, typename Value>
int MultiHashBase<Key, Value>::MultiHashInit(char *base, size_t len,
                                             unsigned int max_level,
                                             unsigned int main_level) {
  shm_mem_base_ = base;
  shm_mem_len_ = len;
  max_level_ = max_level >= 2 * MAX_LEVEL_CNT ? 2 * MAX_LEVEL_CNT : max_level;
  max_level_ = max_level_ <= MAX_LEVEL_CNT / 2 ? MAX_LEVEL_CNT/ 2 : max_level_;
  main_level_ = main_level >= MAIN_LEVEL_CNT ? MAIN_LEVEL_CNT : main_level;
  main_level_ = main_level_<= MAIN_LEVEL_CNT / 2 ? MAIN_LEVEL_CNT / 2 : main_level_;
  main_level_ = main_level_ <= max_level_ ? main_level_ : max_level_ - 1;

  max_item_cnt_ = len / item_size_;
  size_meta_.reserve(max_level_);
  int ret = InitLevelHash();
  if (0 != ret) {
    return -__LINE__;
  }
  return enMultiHashOK;
}

template <typename Key, typename Value>
int MultiHashBase<Key, Value>::InitLevelHash() {
  unsigned int conflict_level = max_level_ - main_level_;
  size_t level_size =
    (size_t)(MAIN_LEVEL_ITEM_RATE * max_item_cnt_ / main_level_);
    gLog("[%s][%d]: level_size %llu rate %f cnt %llu\n", __FILE__, __LINE__, 
          level_size, MAIN_LEVEL_ITEM_RATE, max_item_cnt_);
  
  size_t current_level_size = 0;
  size_t had_push_item_cnt = 0;

  for (int i = 0; i < max_level_; i++) {
    if (i == max_level_ -1) {
      size_meta_[i] = max_item_cnt_ - had_push_item_cnt;
      break;
    }

    if (level_size  <= 2) {
      max_level_ = i + 1;
      size_meta_[i] = max_item_cnt_ - had_push_item_cnt;
      return enMultiHashOK;
    }

    current_level_size = GetBiggestPrimer(level_size);
    size_meta_[i] = current_level_size;
    had_push_item_cnt += current_level_size;
    if (i + 1 == main_level_) {
      level_size = (max_item_cnt_ - had_push_item_cnt) / conflict_level;
    } else {
      level_size = current_level_size - 1;
    }
  }

  for (int i = 0; i < max_level_; i++) {
    gLog("[%s][%d]: level %d size %llu\n", __FILE__, __LINE__, 
          i, size_meta_[i]);
  }
  return enMultiHashOK;
}

template <typename Key, typename Value>
int MultiHashBase<Key, Value>::GetValueForRead(const KeyType &input_key,
                                              ValueType *&value) {
  int     ret = enMultiHashKeyNotFound;
  size_t offset = 0;
  unsigned int  find_level = 0;
  size_t hash_key = HashKey(input_key);
  for (find_level = 0; find_level < max_level_; find_level++) {
    size_t index = hash_key % size_meta_[find_level];

    KeyType* key_in_mem =
      reinterpret_cast<KeyType *>(shm_mem_base_ + offset + (item_size_ * index));
    ValueType* value_in_mem = reinterpret_cast<ValueType *>(
        reinterpret_cast<char *>(key_in_mem) + sizeof(KeyType));

    if (KeyCmp(input_key, *key_in_mem) == true) {
      uint32_t timestamp = 0;
      if (IsExpired(*key_in_mem, *value_in_mem, &timestamp)) {
        ret = enMultiHashKeyNotFound;
        break;
      }
      value = value_in_mem;
      ret = enMultiHashOK;
      AfterFindReadItem(*value_in_mem);
      break;
    } else if (IsEmpty(*key_in_mem, *value_in_mem) == true) {
      ret = enMultiHashKeyNotFound;
      break;
    }
    offset += size_meta_[find_level] * item_size_;
  }

  return ret;
}

template <typename Key, typename Value>
int MultiHashBase<Key, Value>::GetValueForWrite(const KeyType &input_key,
                                                ValueType *&value) {
  int ret = enMultiHashBucketFull;
  ValueType * read_value_ptr;
  ret = GetValueForRead(input_key, read_value_ptr);
  if (ret == enMultiHashOK) {
    value = read_value_ptr;
    AfterFindWriteItem(*value);
    return ret;
  }

  size_t offset  = 0;
  unsigned int  find_level    = 0;
  ret = enMultiHashBucketFull;
  size_t hash_key = HashKey(input_key);
  for (find_level = 0; find_level < max_level_; find_level++) {
    size_t index = hash_key % size_meta_[find_level];
    KeyType* key_in_mem =
      reinterpret_cast<KeyType *>(shm_mem_base_ + offset + (item_size_ * index));
    ValueType* value_in_mem = reinterpret_cast<ValueType *>(
        reinterpret_cast<char *>(key_in_mem) + sizeof(KeyType));

    uint32_t timestamp = 0;
    if (IsEmpty(*key_in_mem, *value_in_mem) == true) {
      if (EmptyOccupy(input_key, *key_in_mem) == true) {
        memset(reinterpret_cast<void *>(value_in_mem), 0 , sizeof(ValueType));
        ret = enMultiHashOK;
        value = value_in_mem;
        break;
      }
      continue;
    } else if (IsExpired(*key_in_mem, *value_in_mem, &timestamp) == true) {
      if (ExpiredOccupy(input_key, *key_in_mem, *value_in_mem, &timestamp) == true) {
        memset(reinterpret_cast<void *>(value_in_mem), 0 , sizeof(ValueType));
        ret = enMultiHashOK;
        value = value_in_mem;
        break;
      }
      continue;
    }
    offset += size_meta_[find_level] * item_size_;
  }
  if (ret == enMultiHashOK) {
    gLog("[%s][%d]: level %d hash %llu\n", __FILE__, __LINE__, 
          find_level, hash_key);
  } else {
    gLog("[%s][%d]: not found hash %llu\n", __FILE__, __LINE__, hash_key);
  }
  return ret;
}

template <typename Key, typename Value>
bool MultiHashBase<Key, Value>::IsPrimer(size_t ullValue) {
  if (0 == (ullValue % 2)) {
    return false;
  }

  size_t ullEnd = (size_t)sqrt(ullValue) + 1;
  if (ullEnd > (ullValue / 2)) {
    ullEnd = ullValue / 2;
  }

  for (size_t i = 3; i <= ullEnd; i++) {
    if (0 == (ullValue % i)) {
        return false;
    }
  }

  return true;
}

template <typename Key, typename Value>
size_t MultiHashBase<Key, Value>::GetBiggestPrimer(
    size_t ullUpperValue) {
  for (size_t i = ullUpperValue; i > 1; i--) {
    if (IsPrimer(i)) {
        return i;
    }
  }
  return 1;
}

}  // namespace libwxfreq

#endif  // FREQLIB_UTIL_MULTI_HASH_BASE_H_

