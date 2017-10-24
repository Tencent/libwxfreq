#include "shm_freq_stat.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include "freq_item.h"
#include "log.h"
#include "reload.h"

namespace libwxfreq {
static uint32_t dummy_timestamp = 0;

bool ShmKeyLessCmp(const FreqKey& left, const FreqKey& right) {
  size_t size = sizeof(left.key);
  uint32_t lef_hash = *reinterpret_cast<const uint32_t*>(
      left.key + size - sizeof(uint32_t));
  uint32_t right_hash = *reinterpret_cast<const uint32_t*>(
      right.key + size - sizeof(uint32_t));
  if (lef_hash < right_hash) return true;
  if (lef_hash > right_hash) return false;
  if (left.appid < right.appid) return true;
  if (left.appid > right.appid) return false;
  return strncmp(right.key, left.key, size) < 0;
}

uint32_t APHash(const char *str) {
  uint32_t hash = 0;
  int i;

  for (i=0; *str; i++) {
    if ((i & 1) == 0) {
      hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
    } else {
      hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
    }
  }
  return (hash & 0x7FFFFFFF);
}

ShmFreqStat::ShmFreqStat(bool zero_init, key_t key, unsigned int item_cnt) :
  shrink_(false), last_shrink_time_(0), item_map_(ShmKeyLessCmp) {
  int ret = table_.MultiHashTableInit(zero_init, key,
                                      S_IRUSR | S_IWUSR, item_cnt);
  if (ret != 0) {
    gLog("[%s][%d]: ShmFreqStat constuct failed, key %u ret %d exit %d\n",
        __FILE__, __LINE__, key, ret, ret);
    exit(ret);
  }
  count_= 0;
  pthread_rwlock_init(&lock_, NULL);
}

void ShmFreqStat::ShrinkMap(time_t now) {
  if (now - last_shrink_time_ > 3600 &&
      __sync_bool_compare_and_swap(&shrink_, false, true)) {
    for (ItemMap::iterator it = item_map_.begin();
        it != item_map_.end(); ++it) {
      ItemMap::iterator dit = it;
      if (static_cast<int>(now - it->second.timestamp) < 90000) {
        FreqItemForShm* freqitemforshm = NULL;
        table_.GetValueForWrite(it->first, freqitemforshm);
        if (freqitemforshm != NULL) {
          memcpy(freqitemforshm, &it->second, sizeof(FreqItemForShm));
          pthread_rwlock_wrlock(&lock_);
          item_map_.erase(dit);
          pthread_rwlock_unlock(&lock_);
        } else {  // TODO(arthurzou)
        }
      } else {
          pthread_rwlock_wrlock(&lock_);
          item_map_.erase(dit);
          pthread_rwlock_unlock(&lock_);
        }
    }
    last_shrink_time_ = now;
    shrink_ = false;
    gLog("[%s][%d]: ShmFreqStat shrinkmap type_name %s size = %u\n",
        __FILE__, __LINE__, type_name().c_str(), item_map_.size());
  }
}

uint32_t& ShmFreqStat::GetItem(const std::string& key, const uint32_t appid,
                              const uint32_t cnt, FreqItem* &freq_item) {
  FreqKey freqkey;
  freqkey.appid = appid;
  uint32_t* key_hash_ptr = reinterpret_cast<uint32_t*>(
      freqkey.key + sizeof(freqkey.key) - sizeof(uint32_t));
  *key_hash_ptr = APHash(key.c_str());
  strncpy(freqkey.key, key.c_str(), sizeof(freqkey.key));

  time_t now = time(NULL);

  FreqItemForShm* freqitemforshm = NULL;
  if (cnt != 0) {
    table_.GetValueForWrite(freqkey, freqitemforshm);
  } else {
    table_.GetValueForRead(freqkey, freqitemforshm);
  }
#ifdef USING_MAP
  ShrinkMap(now);
  if (freqitemforshm == NULL) {  // sad face, find in map
    if (cnt != 0) {
      pthread_rwlock_wrlock(&lock_);
      freqitemforshm = &item_map_[freqkey];
      table_.MarkAsInValid();
      gLog("[%s][%d]: item_map_ type_name %s size %u\n", __FILE__, __LINE__, 
            type_name().c_str(), item_map_.size());
      pthread_rwlock_unlock(&lock_);
      freq_item = reinterpret_cast<FreqItem*>(freqitemforshm);
      return freqitemforshm->timestamp;
    } else {
      pthread_rwlock_rdlock(&lock_);
      ItemMap::iterator it = item_map_.find(freqkey);
      gLog("[%s][%d]: item_map_ type_name %s size %u\n", __FILE__, __LINE__, 
            type_name().c_str(), item_map_.size());
      if (item_map_.size() > 0) {
        table_.MarkAsInValid();
      } else {
        table_.MarkAsValid();
      }
      if (it == item_map_.end() ||
          static_cast<int>(now - it->second.timestamp) >= 90000) {
        freq_item = NULL;
        pthread_rwlock_unlock(&lock_);
        return dummy_timestamp;
      } else {
        freq_item = reinterpret_cast<FreqItem*>(&(it->second));
        pthread_rwlock_unlock(&lock_);
        return it->second.timestamp;
      }
    }
  } else {
    freq_item = reinterpret_cast<FreqItem*>(freqitemforshm);
    if (freq_item->level3_cnt == 0) {
      count_++;
      gLog("[%s][%d]: using shm type_name %s count %u\n", __FILE__, __LINE__, 
          type_name().c_str(), count_);
    }
    return freqitemforshm->timestamp;
  }
#else
  if (freqitemforshm == NULL) {
    freq_item = NULL;
    return dummy_timestamp;
  } else {
    freq_item = reinterpret_cast<FreqItem*>(freqitemforshm);
    return freqitemforshm->timestamp;
  }
#endif
}

ShmFreqStat::~ShmFreqStat() {
}

bool ShmFreqStat::TryReload() {
  if (!table_.IsValid()) {
    if (gLoadFunc(type_name_.c_str()) == 0) {
      table_.MarkAsValid();
      return true;
    } else {
      return false;
    }
  }
  return true;
}

}  // namespace libwxfreq
