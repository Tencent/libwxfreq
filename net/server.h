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

#ifndef LIBFREQ_NET_SERVER_H_
#define LIBFREQ_NET_SERVER_H_

#include <functional>
#include <string>
#include "acceptor.h"
#include "workerpool.h"
#include "server_conf.h"

namespace libwxfreq {

class Server {
 public:
  Server(const char *ip, const int port, Dispatch func,
         const int worker_num = 4);
  explicit Server(Dispatch func);
  void Run();
  WorkerPool& pool();
 private:
  int worker_num_;
  std::string server_ip_;
  int listent_port_;
  WorkerPool pool_;
  Acceptor acceptor_;
};

}  // namespace libwxfreq

#endif  // LIBFREQ_NET_SERVER_H_
