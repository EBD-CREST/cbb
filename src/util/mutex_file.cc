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
#include "mutex_file.h"

#define MUTEX_LOCKED(type, operation) \
  type ret; \
  Mutex::Lock(); \
  { \
    ret = operation; \
  } \
  Mutex::Unlock()

// Mutexファイルクラス
namespace cbb {

/**
 * @breaf constructor
 */
MutexFile::MutexFile() {
  Mutex::Init();
}

/**
 * @breaf destructor
 */
MutexFile::~MutexFile() {
}

/**
 * @see MutexFile::Create
 */
int MutexFile::Create(const char *path, int flags, mode_t mode) {
  MUTEX_LOCKED(
      int,
      FileControl::Create(path, flags, mode)
  );
  return ret;
}

/**
 * @see MutexFile::Open
 */
int MutexFile::Open(const char *path, int flags) {
  MUTEX_LOCKED(
      int,
      FileControl::Open(path, flags)
  );
  return ret;
}

/**
 * @see MutexFile::Read
 */
ssize_t MutexFile::Read(void *buf, size_t size, off_t offset) {
  MUTEX_LOCKED(
      ssize_t,
      FileControl::Read(buf, size, offset)
  );
  return ret;
}

/**
 * @see MutexFile::Write
 */
ssize_t MutexFile::Write(const void *buf, size_t size, off_t offset) {
  MUTEX_LOCKED(
      ssize_t,
      FileControl::Write(buf, size, offset)
  );
  return ret;
}

/**
 * @see MutexFile::FSync
 */
int MutexFile::FSync(int is_data) {
  MUTEX_LOCKED(
      int,
      FileControl::FSync(is_data)
  );
  return ret;
}

/**
 * @see MutexFile::Flush
 */
int MutexFile::Flush() {
  MUTEX_LOCKED(
      int,
      FileControl::Flush()
  );
  return ret;
}

/**
 * @see MutexFile::Close
 */
int MutexFile::Close() {
  MUTEX_LOCKED(
      int,
      FileControl::Close()
  );
  return ret;
}

} /* namespace cbb */
