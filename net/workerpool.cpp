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

#include "workerpool.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "iothread.h"

namespace libwxfreq {
WorkerPool::WorkerPool(Dispatch func, int num)
          :pool_(NULL), pool_size_(num),
          last_accept_worker_index_(0), func_(func) {
}

void WorkerPool::SetWorkerNum(int num) {
  pool_size_ = num;
}

bool WorkerPool::Run() {
    pool_ = static_cast<Thread*>(malloc(pool_size_ * sizeof(Thread)));
    memset(reinterpret_cast<void*>(pool_), 0, pool_size_ * sizeof(Thread));
    for (int i = 0; i < pool_size_; i++) {
        int fds[2] = {0};
        if (pipe(fds) != 0) {
          return false;
        }
        pool_[i].receive_fd = fds[0];
        pool_[i].send_fd = fds[1];
        pool_[i].dispatch = func_;

        pool_[i].iothread = new IOThread(&pool_[i]);
        pool_[i].iothread->run();
    }
    return true;
}

WorkerPool::~WorkerPool() {
    delete[] pool_;
}

void WorkerPool::AddNewConn(int fd) {
    last_accept_worker_index_ = (last_accept_worker_index_ + 1) % pool_size_;
    Thread &cur = pool_[last_accept_worker_index_];
    cur.queue.EnQueue(fd);
    Notify(cur);
}

void WorkerPool::Notify(Thread& thread) {
    int fd = thread.send_fd;
    char cmd[1] = {'c'};
    write(fd, cmd, 1);
}

}  // namespace libwxfreq
