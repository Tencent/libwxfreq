#ifndef FREQLIB_FREQ_FREQ_MANAGER_H_
#define FREQLIB_FREQ_FREQ_MANAGER_H_
#include <stdint.h>
#include <string>
#include <map>

namespace libwxfreq {

class FreqStat;
class AppidMeta;
class FreqItem;
typedef std::map<std::string, FreqStat *> TypeName2FreqStatMap;

class FreqManager {
 public:
  static bool Init(const TypeName2FreqStatMap& type_name_map);
  static const FreqItem* Add(const char* type_name, const std::string& key,
                            const uint32_t appid, const uint32_t cnt);
  static bool AddWhite(const char* type_name, const std::string& key,
                      const uint32_t appid, const uint32_t linger_time);
  static bool DeleteWhite(const char* type_name, const std::string& key,
                        const uint32_t appid);
  static bool AddBlock(const char* type_name, const std::string& key,
                      const uint32_t appid, const uint32_t linger_time,
                      const uint32_t block_level);
  static bool DeleteBlock(const char* type_name, const std::string& key,
                          const uint32_t appid);
  static uint32_t GetWhiteLevel(const FreqItem* item);
  static uint32_t GetBlackLevel(const FreqItem* item);

 private:
  static const uint32_t kWhiteFlag;
  static const uint32_t kAddBlockFlag;
  static const FreqItem* CommAdd(FreqStat *freq_stat, const std::string& key,
                                const AppidMeta& appid_meta,
                                const uint32_t appid, const uint32_t cnt);
  static uint32_t TimeSplit(time_t timestamp, uint32_t scale);
  static TypeName2FreqStatMap type_name_map_;
};

}  // namespace libwxfreq
#endif  // FREQLIB_FREQ_FREQ_MANAGER_H_
