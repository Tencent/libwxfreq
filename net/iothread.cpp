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

#include "iothread.h"
#include <event.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include "util.h"
#include "server_conf.h"

namespace libwxfreq {
using std::string;
using std::max;

static NotifySessionStat notify_stat;
static CloseSessionStat  close_stat;
static ReadSessionStat   read_stat;
static CheckSessionStat   check_stat;
static WriteSessionStat  write_stat;
static ProcessSessionStat process_stat;

static size_t ValidatePkg(const char* buf, size_t len) {
  const char *pn = static_cast<const char*>(memchr(buf, '\n', len));
  const char *pr = static_cast<const char*>(memchr(buf, '\r', len));
  if (pn == NULL && pr == NULL)  {
    return -1;
  }
  const char *p = pn != NULL ? pn : pr;
  if (pr != NULL && pr < p) p = pr;
  while ((*p == '\0' || *p == '\r' || *p == '\n' ||
        *p == ' ' || *p == '\t') && p < buf + len)  p++;
  return p - buf;
}


SKBuff::SKBuff():buffer_(array_), write_pos_(0), read_pos_(0),
          capacity_(LIBFREQ_BUFFER_SIZE) {
            buffer_[0] ='\0';
}

SKBuff::~SKBuff() {
  if (buffer_ != array_) {
    delete[] buffer_;
  }
}

char* SKBuff::GetWritePos() {
  return buffer_ + write_pos_;
}

char* SKBuff::GetReadPos() {
  return buffer_ + read_pos_;
}

size_t SKBuff::AddWritePos(size_t size) {
  write_pos_ += size;
  return write_pos_;
}

size_t SKBuff::AddReadPos(size_t size) {
  read_pos_ += size;
  return read_pos_;
}

size_t SKBuff::GetUseableSize() {
  if (capacity_ - write_pos_ < 32) {
    ExpandSize(capacity_);
  }
  return capacity_ - write_pos_;
}

void SKBuff::Attach(SKBuff& buff) {
  Reset();
  write_pos_ = buff.write_pos_;
  read_pos_ = buff.read_pos_;
  capacity_ = buff.capacity_;
  if (buff.array_ == buff.buffer_) {
    buffer_ = array_;
    memcpy(buffer_, buff.GetReadPos(), buff.GetUnReadSize());
    read_pos_ = 0;
    write_pos_ = buff.GetUnReadSize();
  } else {
    buffer_ = buff.buffer_;
    buff.buffer_ = NULL;
  }
}
void SKBuff::Attach(const char* buff, size_t len) {
  Reset();
  ExpandSize(len);
  memcpy(buffer_, buff, len);
  write_pos_ = len;
}

char* SKBuff::GetBuffer() {
  return buffer_;
}

size_t SKBuff::GetBufferLen() {
  return write_pos_;
}

size_t SKBuff::GetUnReadSize() {
  return write_pos_ - read_pos_;
}

void SKBuff::Reset() {
  if (buffer_ != array_) delete[] buffer_;
  buffer_ = array_;
  write_pos_ = 0;
  read_pos_ = 0;
  capacity_ = LIBFREQ_BUFFER_SIZE;
  buffer_[0] = '\0';
}

void SKBuff::Shrink() {
  memmove(buffer_, buffer_ + read_pos_, GetUnReadSize());
  write_pos_ -= read_pos_;
  read_pos_ = 0;
}

void SKBuff::ExpandSize(size_t len) {
  if (len > capacity_ - write_pos_) {
    capacity_ = max(capacity_ * 2, len + write_pos_);
    char * tmp = new char[capacity_];
    memcpy(tmp, buffer_, write_pos_);
    if (buffer_ != array_) delete[] buffer_;
    buffer_ = tmp;
  }
}

void IOThread::run() {
  thread_->base = event_base_new();
  ConnSession *session = new ConnSession();
  session->fd = thread_->receive_fd;
  session->iothread = this;
  session->stat = &notify_stat;
  SetSockNonBlock(session->fd);
  event_set(&session->event, thread_->receive_fd,
      EV_READ | EV_PERSIST, ConnSession::Handler, session);

  event_base_set(thread_->base, &session->event);

  if (event_add(&session->event, 0) != 0) {
    exit(-1);
  }

  // run thread
  if (pthread_create(&thread_->tid, NULL, IOThread::ThreadEntry, thread_) != 0) {
    exit(-1);
  }
}

IOThread::IOThread(Thread * t)
      :thread_(t) {
}

Thread& IOThread::thread() {
  return *thread_;
}

void* IOThread::ThreadEntry(void *args) {
  Thread *thread = static_cast<Thread *>(args);
  event_base_loop(thread->base, 0);
  event_base_free(thread->base);
  return NULL;
}

void ConnSession::Handler(int fd, int16_t event, void* args) {
  ConnSession *session = static_cast<ConnSession *>(args);
  if (event & EV_TIMEOUT) {
    session->stat = &close_stat;
  }
  while (session->stat->Handler(event, session) > 0) {
    continue;
  }
}

ConnSession::~ConnSession() {
  event_del(&event);
  event_del(&timeout_event);
  close(fd);
}

const std::string& NotifySessionStat::GetName() const  {
  static std::string name = "notify";
  return name;
}

int NotifySessionStat::Handler(int16_t event, ConnSession* session) {
  char buf[1];
  int count = read(session->fd, buf, sizeof(buf));
  if (count <= 0) return kDone;

  ConnItem *item = session->iothread->thread().queue.DeQueue();
  if (item == NULL)  return kDone;

  ConnSession *new_conn_session = new ConnSession();
  new_conn_session->fd = item->fd;
  SetSockNonBlock(item->fd);
  new_conn_session->iothread = session->iothread;
  new_conn_session->stat = &read_stat;
  new_conn_session->keep_alive = false;

  event_set(&new_conn_session->event, new_conn_session->fd,
      EV_READ | EV_PERSIST, ConnSession::Handler, new_conn_session);
  event_base_set(session->iothread->thread().base, &new_conn_session->event);

  event_set(&new_conn_session->timeout_event, -1, 0,
            ConnSession::Handler, new_conn_session);
  event_base_set(session->iothread->thread().base, &new_conn_session->timeout_event);

  ServerConf& conf = ServerConf::GetInstance();
  struct timeval tv = {conf.timeout(), 0};
  if (event_add(&new_conn_session->event, 0) != 0 ||
      event_add(&new_conn_session->timeout_event, &tv) != 0) {
    new_conn_session->stat = &close_stat;
    return kContinue;
  }
  return kDone;
}

const std::string& CloseSessionStat::GetName() const  {
  static std::string name = "close";
  return name;
}

int CloseSessionStat::Handler(int16_t event, ConnSession* session) {
  delete session;
  return kDone;
}

const std::string& ReadSessionStat::GetName() const  {
  static std::string name = "read";
  return name;
}

int ReadSessionStat::Handler(int16_t event, ConnSession* session) {
  int read_size = read(session->fd, session->recv.GetWritePos(), session->recv.GetUseableSize());
  if (read_size <= 0) {
    session->stat = &close_stat;
    return kContinue;
  }
  session->recv.AddWritePos(read_size);
  session->stat = &check_stat;
  return kContinue;
}

const std::string& CheckSessionStat::GetName() const  {
  static std::string name = "check";
  return name;
}

int CheckSessionStat::Handler(int16_t event, ConnSession* session) {
  int pkglen = ValidatePkg(session->recv.GetBuffer(), session->recv.GetBufferLen());
  if (pkglen <= 0) {
    session->stat = &read_stat;
    return kDone;  // continue read more data
  }
  session->in.Attach(session->recv.GetReadPos(), pkglen);
  session->recv.AddReadPos(pkglen);
  session->recv.Shrink();
  session->stat = &process_stat;
  return kContinue;
}

const std::string& WriteSessionStat::GetName() const  {
  static std::string name = "write";
  return name;
}

int WriteSessionStat::Handler(int16_t event, ConnSession* session) {
  size_t size = write(session->fd, session->out.GetReadPos(), session->out.GetUnReadSize());
  if (size < 0) {
    session->stat = &close_stat;
    return kContinue;
  }
  if (size < session->out.GetUnReadSize()) {
    session->out.AddReadPos(size);
    return kDone;  // continue to write;
  }
  if (session->keep_alive) {
    event_del(&session->event);
    event_set(&session->event, session->fd,
        EV_READ | EV_PERSIST, ConnSession::Handler, session);

    event_base_set(session->iothread->thread().base, &session->event);
    if (event_add(&session->event, 0) != 0) {
      session->stat = &close_stat;
      return kContinue;
    }
    session->stat = &check_stat;
    session->out.Reset();
    return kContinue;
  } else {
    session->stat = &close_stat;
    return kContinue;
  }
  return kDone;
}

const std::string& ProcessSessionStat::GetName() const  {
  static std::string name = "process";
  return name;
}

int ProcessSessionStat::Handler(int16_t event, ConnSession* session) {
  if (strncmp(session->in.GetBuffer(), "keep_alive", 10) == 0) {
    session->keep_alive = true;
    session->in.Reset();
    event_del(&session->timeout_event);
    session->stat = &check_stat;
    return kContinue;
  }

  string req(session->in.GetBuffer(), session->in.GetBufferLen());
  string resp;
  session->iothread->thread().dispatch(req, resp);
  session->out.Attach(resp.data(), resp.size());

  event_del(&session->event);
  event_set(&session->event, session->fd,
      EV_WRITE | EV_PERSIST, ConnSession::Handler, session);
  event_base_set(session->iothread->thread().base, &session->event);
  if (event_add(&session->event, 0) != 0) {
    session->stat = &close_stat;
    return kContinue;
  }
  session->stat = &write_stat;
  return kDone;
}

}  // namespace libwxfreq
