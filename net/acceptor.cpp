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

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include "acceptor.h"
#include "server.h"
namespace libwxfreq {
Acceptor::Acceptor(Server* s)
    :s_(s) {
}

void Acceptor::LoopAccept(const char *ip, const int port) {
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);
  if (bind(listen_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
    close(listen_fd);
    exit(-1);
  }
  if (listen(listen_fd, 1024) < 0) {
    close(listen_fd);
    exit(-1);
  }
  while (true) {
    struct sockaddr_in addr_in;
    socklen_t socklen = sizeof(addr_in);
    int accepted_fd = accept(listen_fd, (struct sockaddr*) &addr_in, &socklen);
    if (accepted_fd > 0) {
      s_->pool().AddNewConn(accepted_fd);
    }
  }
  close(listen_fd);
}

}  // namespace libwxfreq
