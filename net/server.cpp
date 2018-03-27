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

#include "server.h"
#include "util.h"

namespace libwxfreq {
Server::Server(const char *ip, const int port, Dispatch func,
                const int worker_num)
            :server_ip_(ip), listent_port_(port), pool_(func, worker_num),
             acceptor_(this) {
}

Server::Server(Dispatch func)
            :pool_(func), acceptor_(this) {
  const ServerConf& conf = ServerConf::GetInstance();
  server_ip_ = conf.listen_ip();
  listent_port_ = conf.listen_port();
  pool_.SetWorkerNum(conf.worker_num());
}

void Server::Run() {
  if (Daemonize() != 0) exit(-1);
  if (!pool_.Run()) exit(-1);
  acceptor_.LoopAccept(server_ip_.c_str(), listent_port_);
}

WorkerPool& Server::pool() {
  return pool_;
}

}  // namespace libwxfreq
