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

#ifndef FREQLIB_NET_SERVER_INNER_H_
#define FREQLIB_NET_SERVER_INNER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <event.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <string>
#include <functional>

namespace libwxfreq {
typedef std::function< void(const std::string&, std::string& resp) > Dispatch;

struct ConnItem {
  int fd;
};

class ConnQueue {
 public:
  ConnQueue() :head_(0), tail_(0) {
  }
  void EnQueue(int fd);
  ConnItem* DeQueue();
 private:
  struct ConnItem array_[1024];
  int head_;
  int tail_;
};

class IOThread;

struct Thread {
  pthread_t  tid;
  IOThread *iothread;
  struct event_base *base;
  int receive_fd;  // worker end
  int send_fd;  // control end
  struct ConnQueue queue;
  Dispatch dispatch;
};

}  // namespace libwxfreq
#endif  // FREQLIB_NET_SERVER_INNER_H_
