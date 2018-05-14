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

#include  <string.h>
#include  <signal.h>
#include  <event.h>
#include  <iostream>
#include  "server.h"
#include  "worker.h"
#include  "libwxfreq.h"

using std::cout;
using std::endl;

static inline int Log(const char* format, ...) {
  return 0;
}

int main(int argc, char* argv[]) {
  libwxfreq::ServerConf& conf = libwxfreq::ServerConf::GetInstance();
  conf.LoadFile("./server.conf");
  SetLogFunc(Log);

  SetRuleConfFile(conf.libwxfreq_conf_path().c_str());
  const std::vector<libwxfreq::StatType>& type = conf.stat_vec();
  for (std::vector<libwxfreq::StatType>::const_iterator it = type.cbegin();
        it != type.cend(); ++it) {
    int ret = RegisterNewShmStat(it->GetTypeName().c_str(), it->init() > 0,
            it->shm_key(), it->item_count());
    if (ret != 0) {
      cout << "RegisterNewShmStat failed, ret = " << ret << endl;
      return -1;
    }
  }

  if (InitFreq() == false) {
    cout << "InitFreq failed" << endl;
    return -2;
  }

  libwxfreq::Server server(libwxfreq::TxtFreqReq);
  // libwxfreq::Server server(ip, port, libwxfreq::Echo, worker_num);
  server.Run();
  return 0;
}
