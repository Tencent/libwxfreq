/*
* Tencent is pleased to support the open source community by making libwxfreq available.
*
* Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the BSD 3-Clause License (the "License"); you may not use this file
* except in compliance with the License. You may obtain a copy of the License at
*
*               https://opensource.org/licenses/BSD-3-Clause
*
* Unless required by applicable law or agreed to in writing, software distributed
* under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
* CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
* language governing permissions and limitations under the License.
*/

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "slice.h"

namespace libwxfreq {


Config::Config(const std::string& conf_filename) {
  conf_filename_ = conf_filename;
}

const std::string & Config::conf_filename() {
  return conf_filename_;
}

void Config::set_conf_filename(const std::string &conf_filename) {
  conf_filename_ = conf_filename;
}

int Config::Init(void) {
  int ret = LoadFile();
  if (0 != ret) {
    gLog("[%s][%d]: init config %s error return %d\n", __FILE__, __LINE__,
        conf_filename_.c_str(), ret);
    Reset();
  }
  return ret;
}

int Config::Reset(void) {
  conf_filename_buf_.clear();
  section_list_.clear();
  table_.Clear();
}


int Config::ParserConfig() {
  Slice section, key, value;
  const char *eol = NULL, *tmp = NULL;
  const char *ptr = conf_filename_buf_.c_str();
  const char *endptr = conf_filename_buf_.c_str() + conf_filename_buf_.size();

  while (ptr < endptr) {
    eol = strchr(ptr, '\n');  // getline
    if (eol == NULL) eol = endptr;  // last line
    while (*ptr == ' ' || *ptr == '\t') ptr++;  // ltrim

    if (*ptr == '[') {  // section
      ptr += 1;
      tmp = ptr;
      while (*ptr != ']' && ptr < eol) ptr++;
      section = Slice(tmp, ptr);
      section.StrTrim("\t ");
      section_list_.push_back(section);
    } else if (NULL == strchr("#;\n", *ptr)) {  // item
      tmp = ptr;
      while (*ptr != '=' && ptr < eol) ptr++;
      key = Slice(tmp, ptr);
      key.StrTrim("\t ");

      if (*ptr == '=' && section.start() != NULL) {
        ptr += 1;
        tmp = ptr;
        while (ptr < eol) ptr++;
        value = Slice(tmp, ptr);
        value.StrTrim("\t ");
        table_.Add(section, key, value);
      }
    }
    ptr = eol + 1;  // next line
  }
  return section_list_.size() > 0 ? 0 : -1;
}

int Config::LoadFile(void) {
  FILE * fp = fopen(conf_filename_.c_str(), "r");
  if (NULL != fp) {
    struct stat fileStat;
    if (0 == fstat(fileno(fp), &fileStat)) {
      Reset();
      if (fileStat.st_size == 0) return 0;
      char *tmp = reinterpret_cast<char*>(malloc(fileStat.st_size + 64));
      fread(tmp, fileStat.st_size, 1, fp);
      tmp[fileStat.st_size] = '\0';  // append '\0' make strchr happy
      conf_filename_buf_ = tmp;
      free(tmp);
      ParserConfig();
    } else {
      gLog("[%s][%d]: open confile %s error\n",
          __FILE__, __LINE__, conf_filename_.c_str());
    }
    fclose(fp);
    fp = NULL;
  }
  return section_list_.size() == 0 ? -1 : 0;
}


void Config::section_list(std::vector<std::string>& sectionlist) {
  for (size_t i = 0; i < section_list_.size(); i++) {
    sectionlist.push_back(std::string(section_list_[i].start(),
                        section_list_[i].end() - section_list_[i].start()));
  }
}

void Config::GetKeysBySection(const std::string &section,
                              std::vector<std::string>& keys) const {
  std::vector<Slice> slice_keys;
  table_.GetKeysBySection(section, slice_keys);
  for (std::vector<Slice>::iterator it = slice_keys.begin();
      it != slice_keys.end(); ++it) {
    keys.push_back(it->ToStr());
  }
}


int Config::ReadItem(const std::string& section, const std::string& key,
                    const char* defaultvalue, std::string& itemvalue) const {
  if (section.size() <= 0 || key.size() <= 0 ) return -1;
  std::string value;
  Slice idxsec = section;
  Slice idxkey = key;
  Slice val = table_.Get(idxsec, idxkey);

  if (val.start() != NULL) {
    value.assign(val.start(), val.end() - val.start());
    std::stringstream ss(value, std::ios_base::in);
    itemvalue = ss.str();
    return 0;
  } else {
    std::stringstream ss;
    ss << defaultvalue;
    itemvalue = ss.str();
    return 1;
  }
}

}  // namespace libwxfreq

