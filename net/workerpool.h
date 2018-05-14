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

#ifndef LIBFREQ_NET_WORKERPOOL_H_
#define LIBFREQ_NET_WORKERPOOL_H_
#include "server_inner.h"
namespace libwxfreq {
class WorkerPool {
 public:
  explicit WorkerPool(Dispatch func, int num = 0);
  void SetWorkerNum(int num);
  ~WorkerPool();
  void AddNewConn(int fd);
  bool Run();
 private:
  void Notify(Thread& thread);
 private:
  Thread* pool_;
  int pool_size_;
  size_t last_accept_worker_index_;
  Dispatch func_;
};

}  // namespace libwxfreq
#endif  // LIBFREQ_NET_WORKERPOOL_H_
