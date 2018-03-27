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

#include <string.h>
#include <cstring>
#include <string>
#include <sstream>
#include <cstdio>
#include <vector>

#include "libwxfreq.h"
namespace libwxfreq {
using std::string;
using std::vector;
using std::ostringstream;

void Echo(const string& req, string& resp) {
  resp = req;
}

void SplitReq(const char* req, vector<string>& cmd) {
  const char* end = req;
  const char* start = NULL;

  while(*end != '\n' && *end != '\r' && *end != '\0') {
    if (*end == ' ' || *end == '\t') {
       if (start != NULL) {
         cmd.push_back(string(start, end - start));
         start = NULL;
       }
    } else {
      if (start == NULL) {
        start = end;
      }
    }
    end++;
  }
  if (start != NULL) {
    cmd.push_back(string(start, end - start));
  }
}


void TxtFreqReq(const string& req, string& resp) {
  vector<string> parsed_req;
  SplitReq(req.c_str(), parsed_req);

  if (parsed_req.size() == 0) return;
  string cmd = parsed_req[0];
  ostringstream ostr;

  if (strcasecmp(cmd.c_str(), "ReportAndCheck") == 0) {
    if (parsed_req.size() != 5) {
      ostr << "{\"code\": -1, \"msg\":\"wrong cmd, [ReportAndCheck type user key appid cnt]\"}\n\n";
    } else {
      uint32_t appid = strtoul(parsed_req[3].c_str(), NULL, 10);
      uint32_t cnt = strtoul(parsed_req[4].c_str(), NULL, 10);
      BlockResult bs = ReportAndCheck(parsed_req[1].c_str(),
                   parsed_req[2].c_str(), appid, cnt);
      ostr << "{\"code\": 0, \"block_level\": " << bs.block_level << ", \"match_rule\": \"" << bs.match_rule <<"\"}\n\n";
    }
  } else if (strcasecmp(cmd.c_str(), "OnlyCheck") == 0) {
    if (parsed_req.size() != 4) {
      ostr << "{\"code\": -1, \"msg\":\"wrong cmd, [OnlyCheck type user key appid]\"}\n\n";
    } else {
      uint32_t appid = strtoul(parsed_req[3].c_str(), NULL, 10);
      BlockResult bs = OnlyCheck(parsed_req[1].c_str(),
                 parsed_req[2].c_str(), appid);
      ostr << "{\"code\": 0, \"block_level\": " << bs.block_level << ", \"match_rule\": \"" << bs.match_rule << "\"}\n\n";
    }
  } else if (strcasecmp(cmd.c_str(), "OnlyReport") == 0) {
    if (parsed_req.size() != 5) {
      ostr << "{\"code\": -1, \"msg\":\"wrong cmd, [OnlyReport type user key appid cnt]\"}\n\n";
    } else {
      uint32_t appid = strtoul(parsed_req[3].c_str(), NULL, 10);
      uint32_t cnt = strtoul(parsed_req[4].c_str(), NULL, 10);
      int ret = OnlyReport(parsed_req[1].c_str(),
                 parsed_req[2].c_str(), appid, cnt);
      ostr << "{\"code\": " << ret << "}\n\n";
    }
  } else if (strcasecmp(cmd.c_str(), "GetCache") == 0) {
    if (parsed_req.size() != 4) {
      ostr << "{\"code\": -1, \"msg\":\"wrong cmd, [GetCache type user key appid]\"}\n\n";
    } else {
      uint32_t appid = strtoul(parsed_req[3].c_str(), NULL, 10);
      FreqCache cache = GetCache(parsed_req[1].c_str(),
                   parsed_req[2].c_str(), appid);
      ostr << "{\"code\": 0, \"cnt1\": " << cache.level1_cnt <<", \"cnt2\": "
          << cache.level2_cnt <<", \"cnt3\": " << cache.level3_cnt << "}\n\n";
    }
  } else if (strcasecmp(cmd.c_str(), "AddWhite") == 0) {
    if (parsed_req.size() != 5) {
      ostr << "{\"code\": -1, \"msg\":\"wrong cmd, [AddWhite type user key appid linger_time]\"}\n\n";
    } else {
      uint32_t appid = strtoul(parsed_req[3].c_str(), NULL, 10);
      uint32_t linger_time = strtoul(parsed_req[4].c_str(), NULL, 10);
      int ret = AddWhite(parsed_req[1].c_str(), parsed_req[2].c_str(), appid, linger_time);
      ostr << "{\"code\": " << ret << "}\n\n";
    }
  } else if (strcasecmp(cmd.c_str(), "DeleteWhite") == 0) {
    if (parsed_req.size() != 4) {
      ostr << "{\"code\": -1, \"msg\":\"wrong cmd, [DeleteWhite type user key appid]\"}\n\n";
    } else {
      uint32_t appid = strtoul(parsed_req[3].c_str(), NULL, 10);
      int ret = DeleteWhite(parsed_req[1].c_str(), parsed_req[2].c_str(), appid);
      ostr << "{\"code\": " << ret << "}\n\n";
    }
  } else if (strcasecmp(cmd.c_str(), "AddBlock") == 0) {
    if (parsed_req.size() != 6) {
      ostr << "{\"code\": -1, \"msg\":\"wrong cmd, [AddBlock type user key appid linger_time block_level]\"}\n\n";
    } else {
      uint32_t appid = strtoul(parsed_req[3].c_str(), NULL, 10);
      uint32_t linger_time = strtoul(parsed_req[4].c_str(), NULL, 10);
      uint32_t block_level = strtoul(parsed_req[5].c_str(), NULL, 10);
      int ret = AddBlock(parsed_req[1].c_str(), parsed_req[2].c_str(), appid, linger_time, block_level);
      ostr << "{\"code\": " << ret << "}\n\n";
    }
  } else if (strcasecmp(cmd.c_str(), "DeleteBlock") == 0) {
    if (parsed_req.size() != 4) {
      ostr << "{\"code\": -1, \"msg\":\"wrong cmd, [DeleteBlock type user key appid]\"}\n\n";
    } else {
      uint32_t appid = strtoul(parsed_req[3].c_str(), NULL, 10);
      int ret = DeleteBlock(parsed_req[1].c_str(), parsed_req[2].c_str(), appid);
      ostr << "{\"code\": " << ret << "}\n\n";
    }
  } else if (strcasecmp(cmd.c_str(), "Help") == 0) {
      ostr << "cmdlist:\n1: ReportAndCheck\n2: OnlyCheck\n3: OnlyReport\n"
                    "4: GetCache\n5: AddWhite\n6: DeleteWhite\n7: AddBlock\n"
                    "8: DeleteBlock\n\n";
  } else {
      ostr << "{\"code\": -1, \"msg\":\"unkown cmd " << cmd << ", try [help]\"}\n\n";
  }

  resp = ostr.str();
}

}  // namespace libwxfreq
