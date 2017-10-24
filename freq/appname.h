#ifndef FREQLIB_FREQ_APPNAME_H_
#define FREQLIB_FREQ_APPNAME_H_

#include <stdint.h>
#include <stdio.h>
#include <string>

namespace libwxfreq {

class AppNameMeta {
 public:
  AppNameMeta()
    : appname_(),
      appid_(0),
      min_interval_(60),
      mid_interval_(3600),
      max_interval_(86400) {
  }

  inline void set_min_interval(const uint32_t& min_interval) {
    min_interval_ = min_interval;
  }

  inline void set_mid_interval(const uint32_t& mid_interval) {
    mid_interval_ = mid_interval;
  }

  inline void set_max_interval(const uint32_t& max_interval) {
    max_interval_ = max_interval;
  }

  inline void set_appid(const uint32_t& appid) {
    appid_ = appid;
  }

  inline void set_appname(const std::string& appname) {
    appname_ = appname;
  }

  inline std::string appname() const {
    return appname_;
  }

  inline uint16_t min_interval() const {
    return min_interval_;
  }

  inline uint16_t mid_interval() const {
    return mid_interval_;
  }
  inline uint16_t max_interval() const {
    return max_interval_;
  }

  inline uint32_t appid() const {
    return appid_;
  }

  inline std::string DebugString() const {
    char buf[128];
    snprintf(buf, sizeof(buf), "appname=%s, appid=%d, min_interval=%d, "
        "mid_interval=%d, max_interval=%d", appname_.c_str(), appid_,
        min_interval_, mid_interval_, max_interval_);
    return buf;
  }

 private:
  std::string appname_;
  uint32_t appid_;
  uint16_t min_interval_;
  uint16_t mid_interval_;
  uint32_t max_interval_;
};

}  // namespace libwxfreq

#endif  // FREQLIB_FREQ_APPNAME_H_
