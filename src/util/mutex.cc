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
#include "mutex.h"

// Mutexクラス
namespace cbb {

/**
 * @breaf Mutex初期化
 */
void Mutex::Init() {
  pthread_mutexattr_t attr;

  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_TIMED_NP);

  pthread_mutex_init(&mutex_, &attr);

  pthread_mutexattr_destroy(&attr);
}

/**
 * @breaf Mutexのロック
 */
void Mutex::Lock() {
  pthread_mutex_lock(&mutex_);
}

/**
 * @breaf Mutexのアンロック
 */
void Mutex::Unlock() {
  pthread_mutex_unlock(&mutex_);
}

} /* namespace cbb */
