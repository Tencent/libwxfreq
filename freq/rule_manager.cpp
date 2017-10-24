#include "rule_manager.h"
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "plain_expression.h"
#include "config.h"
#include "log.h"
#include "appid_manager.h"
#include "freq_item.h"
#include "rule_item.h"
#include "freq_manager.h"

namespace libwxfreq {

std::string RuleManager::rule_conf_file_;
bool RuleManager::is_loading_ = false;
time_t RuleManager::last_modify_time_ = 0;
unsigned char RuleManager::index_ = 0;
TypeNameMap RuleManager::map_[2];

int RuleManager::Init() {
  return TryLoadFile();
}

int RuleManager::TryLoadFile() {
  if (rule_conf_file_.empty()) {
    gLog("[%s][%d]: rule confile empty\n", __FILE__, __LINE__);
    return -1;
  }

  struct stat _filestat;
  if (stat(rule_conf_file_.c_str(), &_filestat) < 0) {
    gLog("[%s][%d]: stat confile %s failed\n",
        __FILE__, __LINE__, rule_conf_file_.c_str());
    return -2;
  }
  if (_filestat.st_mtime > last_modify_time_ &&
      time(NULL) - _filestat.st_mtime >= 2) {
    if (__sync_bool_compare_and_swap(&is_loading_, false, true)) {
      last_modify_time_ = _filestat.st_mtime;
      LoadFile();
      is_loading_ = false;
    }
  }
  return 0;
}

int RuleManager::LoadFile() {
  gLog("[%s][%d]: Load confile %s\n",
      __FILE__, __LINE__, rule_conf_file_.c_str());
  Config cfg(rule_conf_file_);
  int ret = 0;
  if (cfg.Init() != 0) {
    gLog("[%s][%d]: int confile %s failed\n",
        __FILE__, __LINE__, rule_conf_file_.c_str());
    return -1;
  }

  unsigned char new_index = (index_ + 1) % 2;
  map_[new_index].clear();

  std::vector<std::string> sectionList;
  cfg.section_list(sectionList);
  for (std::vector<std::string>::iterator it = sectionList.begin();
      it != sectionList.end(); ++it) {
      if (*it != "appid") ret = InsertRule(cfg, it->c_str());
  }

  AppidManager::UpdateAppidMeta(cfg);

  index_ = new_index;
  gLog("[%s][%d]: new index %d\n", __FILE__, __LINE__, index_);
  return 0;
}

int RuleManager::InsertRule(const Config& config, const std::string& section) {
  std::string item;
  std::string rule_type;
  unsigned int block_level = 0;
  unsigned int match_appid;
  unsigned int new_index = (index_ + 1) % 2;

  int ret1 = config.ReadItem(section, "item", "", item);
  int ret2 = config.ReadItem(section, "match_appid", 0, match_appid);
  int ret3 = config.ReadItem(section, "block_level", 0, block_level);
  int ret4 = config.ReadItem(section, "rule_type", "user", rule_type);

  if (ret1 != 0 || ret2 != 0 || ret3 != 0 || ret4 < 0) {
    gLog("[%s][%d]: insert rule section %s failed %d|%d|%d|%d\n",
        __FILE__, __LINE__, section.c_str(), ret1, ret2, ret3, ret4);
    return -1;
  }

  if (block_level <= 0) {
    gLog("[%s][%d]: insert rule section %s block_level %d, skip\n",
        __FILE__, __LINE__, section.c_str(), block_level);
    return -2;
  }

  RuleItem rule_item(section, block_level);
  rule_item.set_expression(new PlainExpression());
  if (rule_item.ParseItem(item) == false) {
    gLog("[%s][%d]: insert rule section %s parse failed skip\n",
        __FILE__, __LINE__, section.c_str());
    return -3;
  }

  map_[new_index][rule_type][match_appid].push_back(rule_item);

  gLog("[%s][%d]: insert rule section %s match_appid %u block_level %d"
      " rule_type %s item %s rule_item %s\n", __FILE__, __LINE__,
      section.c_str(), match_appid, block_level, rule_type.c_str(),
      item.c_str(), rule_item.DebugString().c_str());
  return 0;
}

std::string RuleManager::DebugString() {
  TryLoadFile();

  TypeNameMap& current_map = map_[index_];
  std::string rule_string;
  for (TypeNameMap::iterator cit = current_map.begin();
      cit != current_map.end(); ++cit) {
    for (AppidMapVecRuleItem::iterator it = cit->second.begin();
        it != cit->second.end(); ++it) {
      for (std::vector<RuleItem>::iterator vit = it->second.begin();
          vit != it->second.end(); ++vit) {
        rule_string.append("match_appid:\t");
        rule_string.append(std::to_string(it->first));
        rule_string.append("\n");
        rule_string.append(vit->DebugString());
      }
    }
  }
  return rule_string;
}

RuleManager::Result RuleManager::IsMatch(const uint32_t appid,
                                        const FreqItem& freq_item,
                                        const AppidMapVecRuleItem& map) {
  TryLoadFile();

  AppidMapVecRuleItem::const_iterator it = map.find(appid);
  if (it == map.end()) {
    return RuleManager::Result(0, "");
  }

  for (std::vector<RuleItem>::const_iterator vit = it->second.begin();
      vit != it->second.end(); ++vit) {
    if (vit->IsMatch(freq_item)) {
      RuleManager::Result res(vit->block_level(), vit->match_rulename());
      return res;
    }
  }
  return RuleManager::Result(0, "");
}

RuleManager::Result RuleManager::IsMatchRule(const char * type_name,
                                            const uint32_t appid,
                                            const FreqItem& freq_item) {
  if (type_name == NULL || FreqManager::GetWhiteLevel(&freq_item) > 0) {
    return RuleManager::Result(0, "");
  }
  if (FreqManager::GetBlackLevel(&freq_item) > 0) {
    return RuleManager::Result(FreqManager::GetBlackLevel(&freq_item), "black");
  }
  TypeNameMap::iterator it = map_[index_].find(type_name);
  if (it == map_[index_].end()) {
    return RuleManager::Result(0, "");
  }
  return IsMatch(appid, freq_item, it->second);
}

}  // namespace libwxfreq
