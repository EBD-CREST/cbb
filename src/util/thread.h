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
#ifndef UTIL_THREAD_H_
#define UTIL_THREAD_H_

#include <stdint.h>
#include <pthread.h>
#include "mutex.h"

namespace cbb {

// スレッド制御クラス
class Thread : public Mutex {
 public:
  Thread();
  virtual ~Thread();

  pthread_t Create(void *user_data, uint64_t interval_time = 0);
  void Release();

 protected:
  void Init();
  virtual bool ThreadCall(void *user_data) = 0;

 private:
  /// スレッド情報
  struct ThreadInfo {
    bool is_loop;
    uint64_t interval_time;
    Thread *thread_ptr;
    void *user_data;
  };

  static void *Threading(void *data);

  ThreadInfo thread_info_;
  pthread_t thread_id_;
};

} /* namespace cbb */

#endif /* UTIL_THREAD_H_ */
