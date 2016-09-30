//
// Copyright (C) 2015 Tokyo Institute of Technology
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "thread.h"

#include "../common/common.h"

#define THREAD_CALL_SLEEP     5 // sec

// スレッド制御クラス
namespace cbb {

/**
 * @breaf スレッドループ用static関数
 * @param data ユーザーデータ
 * @return NULL
 */
void *Thread::Threading(void *data)
{
  struct ThreadInfo *info = (struct ThreadInfo *)data;
  uint64_t now_time, next_time = 0;
  bool result = true;

  while (info->is_loop && result) {
    now_time = get_time_msec();

//    DMSG("Thread::Threading : time = %d / %d : loop = %d\n", now_time, next_time, (int)info->is_loop);

    if (next_time <= now_time) {
      next_time = now_time + info->interval_time;
      info->thread_ptr->Lock();
      result = info->thread_ptr->ThreadCall(info->user_data);
      info->thread_ptr->Unlock();
    } else {
      sleep(THREAD_CALL_SLEEP);
    }
  }

  return (void *)NULL;
}

/**
 * @breaf constractor
 */
Thread::Thread() {
  Init();
}

/**
 * @breaf destructor
 */
Thread::~Thread() {
}

/**
 * @breaf スレッド作成
 * @param user_data ユーザーデータ
 * @return pthread_t スレッドID
 */
pthread_t Thread::Create(void *user_data, uint64_t interval_time) {
  Release();

  pthread_create(&thread_id_, NULL, cbb::Thread::Threading, &thread_info_);

  if (thread_id_ != 0) {
    Mutex::Init();

    thread_info_.is_loop = true;
    thread_info_.interval_time = interval_time;
    thread_info_.thread_ptr = this;
    thread_info_.user_data = user_data;
  }

  return thread_id_;
}

/**
 * @breaf スレッド開放
 */
void Thread::Release() {
  thread_info_.is_loop = false;

  if (thread_id_ != 0) {
    pthread_join(thread_id_, NULL);
    thread_id_ = 0;
  }

  Init();
}

/**
 * @breaf 初期化
 */
void Thread::Init() {
  thread_id_ = 0;
  thread_info_.is_loop = false;
  thread_info_.thread_ptr = NULL;
  thread_info_.user_data = NULL;
}

} /* namespace cbb */
