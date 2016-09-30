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
#include "file_control.h"
#include "common/error.h"

// ファイル制御クラス
namespace cbb {

/**
 * @breaf constructor
 */
FileControl::FileControl() {
  fd_ = -1;
}

/**
 * @breaf copy-constructor
 * @param fd ファイルディスクリプタ
 */
FileControl::FileControl(int fd) {
  fd_ = fd;
}

/**
 * @breaf destructor
 */
FileControl::~FileControl() {
}

/**
 * @breaf ファイル作成
 * @param path ファイルパス
 * @param flag ファイルフラグ
 * @param mode ファイルモード
 * @return ファイルディスクリプタ
 */
int FileControl::Create(const char *path, int flags, mode_t mode) {
  assert(fd_ == -1);

  int fd = open(path, flags, mode);
  int lock_mode = 0;

  // openに成功した場合
  if (fd != -1) {
    // 書き込み属性がある場合
    if (flags & (O_WRONLY | O_RDWR) != 0) {
      lock_mode = LOCK_EX;
    } else {
      lock_mode = LOCK_SH;
    }

    // ロック設定
//    if (flock(fd, lock_mode) != 0) {
//      close(fd);
//      fd = -1;
//    }
  }

  fd_ = fd;

  return fd;
}

/**
 * @breaf ファイルオープン
 * @param path ファイルパス
 * @param flag ファイルフラグ
 * @return ファイルディスクリプタ
 */
int FileControl::Open(const char *path, int flags) {
  assert(fd_ == -1);

  int fd = open(path, flags);
  int lock_mode = 0;

  // openに成功した場合
  if (fd != -1) {
    // 書き込み属性がある場合
    if (flags & (O_WRONLY | O_RDWR) != 0) {
      lock_mode = LOCK_EX;
    } else {
      lock_mode = LOCK_SH;
    }

    // ロック設定
//    if (flock(fd, lock_mode) != 0) {
//      close(fd);
//      fd = -1;
//    }
  }

  fd_ = fd;

  return fd;
}

/**
 * @breaf ファイル読み込み
 * @param buf バッファポインタ
 * @param size 読み込みサイズ
 * @param offset オフセット
 * @return 読み込みサイズ
 */
ssize_t FileControl::Read(void *buf, size_t size, off_t offset) {
  assert(fd_ != -1);
  assert(buf != NULL);

  ssize_t ssize = pread(fd_, buf, size, offset);
  if (ssize == -1) {
    ssize = errno_to_cbb_error(ssize);
  }

  return ssize;
}

/**
 * @breaf ファイル書き込み
 * @param buf バッファポインタ
 * @param size バッファサイズ
 * @param offset オフセット
 * @return 書き込みサイズ
 */
ssize_t FileControl::Write(const void *buf, size_t size, off_t offset) {
  assert(fd_ != -1);
  assert(buf != NULL);

  ssize_t ssize = pwrite(fd_, buf, size, offset);
  if (ssize == -1) {
    ssize = errno_to_cbb_error(ssize);
  }

  return ssize;
}

/**
 * @breaf ファイル同期
 * @param is_data Data Syncかどうか
 * @return 結果
 */
int FileControl::FSync(int is_data) {
  assert(fd_ != -1);

  int ret;
  if (is_data) {
    ret = fdatasync(fd_);
  } else {
    ret = fsync(fd_);
  }
  return ret;
}

/**
 * @breaf ファイルフラッシュ
 * @param fd ファイルディスクリプタ
 * @return 結果
 */
int FileControl::Flush() {
  assert(fd_ != -1);

  return close(dup(fd_));
}

/**
 * @breaf ファイルクローズ
 * @param fd ファイルディスクリプタ
 * @return 結果
 */
int FileControl::Close() {
  assert(fd_ != -1);

//  flock(fd_, LOCK_UN);
  int ret = close(fd_);

  fd_ = -1;

  return ret;
}

} /* namespace cbb */
