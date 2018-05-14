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

#ifndef LIBFREQ_NET_IOTHREAD_H_
#define LIBFREQ_NET_IOTHREAD_H_
#include <string>
#include <cstdint>
#include "server_inner.h"

#define LIBFREQ_BUFFER_SIZE 128
namespace libwxfreq {
class IOThread {
 public:
  explicit IOThread(Thread * t);
  void run();
  Thread& thread();
 private:
  static void* ThreadEntry(void *);
  Thread *thread_;
};

class ConnSession;
class SessionStat {
 public:
  virtual const std::string& GetName() const = 0;
  virtual int Handler(int16_t event, ConnSession* session) = 0;
 protected:
  enum {
    kDone = 0,
    kContinue = 1,
  };
};

class NotifySessionStat:public SessionStat {
 public:
  const std::string& GetName() const;
  int Handler(int16_t event, ConnSession* session);
};

class CloseSessionStat:public SessionStat {
 public:
  const std::string& GetName() const;
  int Handler(int16_t event, ConnSession* session);
};

class ReadSessionStat:public SessionStat {
 public:
  const std::string& GetName() const;
  int Handler(int16_t event, ConnSession* session);
};

class CheckSessionStat:public SessionStat {
 public:
  const std::string& GetName() const;
  int Handler(int16_t event, ConnSession* session);
};

class WriteSessionStat:public SessionStat {
 public:
  const std::string& GetName() const;
  int Handler(int16_t event, ConnSession* session);
};

class ProcessSessionStat:public SessionStat {
 public:
  const std::string& GetName() const;
  int Handler(int16_t event, ConnSession* session);
};

class SKBuff {
 public:
  SKBuff();
  ~SKBuff();
  char* GetWritePos();
  char* GetReadPos();
  size_t GetUseableSize();
  void Attach(SKBuff& buff);
  void Attach(const char* buff, size_t len);
  char* GetBuffer();
  size_t GetBufferLen();
  size_t GetUnReadSize();
  void Reset();
  void Shrink();
  size_t AddWritePos(size_t size);
  size_t AddReadPos(size_t size);
 private:
  void ExpandSize(size_t len);
 private:
  char array_[LIBFREQ_BUFFER_SIZE];
  char* buffer_;
  size_t write_pos_;
  size_t read_pos_;
  size_t capacity_;
};

struct ConnSession {
  int fd;
  bool    keep_alive;
  SessionStat  *stat;
  SKBuff  recv;
  SKBuff  in;
  SKBuff  out;
  IOThread*   iothread;
  struct event event;
  struct event timeout_event;
  static void Handler(int, int16_t, void *);
  ~ConnSession();
};
}  // namespace libwxfreq

#endif  // LIBFREQ_NET_IOTHREAD_H_
