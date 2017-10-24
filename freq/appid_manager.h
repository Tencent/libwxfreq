#ifndef FREQLIB_FREQ_APPID_MANAGER_H_
#define FREQLIB_FREQ_APPID_MANAGER_H_
#include <vector>
#include <map>
#include <string>
#include "appid.h"

namespace libwxfreq {
class Config;

typedef std::map<uint32_t, AppidMeta> AppidMapVecAppidMeta;

class AppidManager {
 public:
  static const AppidMeta* GetAppidMeta(const uint32_t appid);
  static void UpdateAppidMeta(const Config& config);

 private:
  static AppidMapVecAppidMeta appid_map_[2];
  static uint8_t index_;
};

}  // namespace libwxfreq
#endif  // FREQLIB_FREQ_APPID_MANAGER_H_
