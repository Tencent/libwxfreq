#ifndef FREQLIB_UTIL_FREQ_STAT_H_
#define FREQLIB_UTIL_FREQ_STAT_H_
#include <stdint.h>
#include <string>

namespace libwxfreq {
class FreqItem;

class FreqStat {
 public:
  virtual uint32_t& GetItem(const std::string& key, const uint32_t appid,
                            const uint32_t cnt, FreqItem* &freq_item) = 0;
  virtual bool TryReload() = 0;
  void set_type_name(const std::string& type_name) {
    type_name_ = type_name;
  }
  
  const std::string& type_name() const {
    return type_name_;
  }

  virtual ~FreqStat() { }
 protected:
  std::string type_name_;
};

}  // namespace libwxfreq

#endif  // FREQLIB_UTIL_FREQ_STAT_H_
