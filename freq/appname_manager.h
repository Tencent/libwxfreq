#ifndef FREQLIB_FREQ_APPNAME_MANAGER_H_
#define FREQLIB_FREQ_APPNAME_MANAGER_H_
#include <vector>
#include <map>
#include <string>
#include "appname.h"

namespace libwxfreq {
class Config;

typedef std::map<std::string, AppNameMeta> AppanmeMapVecAppnameMeta;

class AppnameManager {
 public:
  static const AppNameMeta* GetAppNameMeta(const std::string& appname);
  static void UpdateAppNameMeta(const Config& config);

 private:
  static AppanmeMapVecAppnameMeta appname_map_[2];
  static uint8_t index_;
};

}  // namespace libwxfreq
#endif  // FREQLIB_FREQ_APPNAME_MANAGER_H_
