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

#include "server_inner.h"

namespace libwxfreq {
void ConnQueue::EnQueue(int fd) {
  if ((head_ + 1) % 1024 != tail_) {
    array_[head_++].fd = fd;
    head_ %= 1024;
  }
}

ConnItem* ConnQueue::DeQueue() {
  ConnItem* item = NULL;
  if (tail_ != head_) {
    item = &array_[tail_++];
    tail_ %= 1024;
  }
  return item;
}

}  // namespace libwxfreq
