#ifndef FREQLIB_FREQ_RULE_MANAGER_H_
#define FREQLIB_FREQ_RULE_MANAGER_H_
#include <pthread.h>
#include <vector>
#include <map>
#include <string>

namespace libwxfreq {

class Config;
class FreqItem;
class RuleItem;

typedef std::map<unsigned int, std::vector<RuleItem> > AppidMapVecRuleItem;
typedef std::map<std::string, AppidMapVecRuleItem> TypeNameMap;

class RuleManager {
 public:
  struct Result {
    int block_level;
    std::string match_rule;

    Result(): block_level(0), match_rule() { }
    Result(int b, const std::string& matchrule)
      : block_level(b),
        match_rule(matchrule) { }
  };

 public:
  static inline void set_rule_conf_file(const std::string& filename) {
    rule_conf_file_ = filename;
  }
  static int Init();

  static std::string DebugString();
  static Result IsMatchRule(const char * type_name, const uint32_t appid,
                            const FreqItem& freq_item);

 private:
  static int LoadFile();
  static int TryLoadFile();
  static int InsertRule(const Config& config, const std::string& section);

  static Result IsMatch(const uint32_t appid, const FreqItem& freq_item,
                        const AppidMapVecRuleItem& map);

 private:
  static std::string rule_conf_file_;
  static bool is_loading_;
  static time_t last_modify_time_;

  static TypeNameMap map_[2];
  static unsigned char index_;
};

}  // namespace libwxfreq
#endif  // FREQLIB_FREQ_RULE_MANAGER_H_
