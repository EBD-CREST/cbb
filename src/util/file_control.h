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
#ifndef UTIL_FILE_CONTROL_H_
#define UTIL_FILE_CONTROL_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>

namespace cbb {

// ファイル制御クラス
class FileControl {
 public:
  FileControl();
  FileControl(int fd);
  virtual ~FileControl();

  int Create(const char *path, int flags, mode_t mode);
  int Open(const char *path, int flags);
  ssize_t Read(void *buf, size_t size, off_t offset);
  ssize_t Write(const void *buf, size_t size, off_t offset);
  int FSync(int isData);
  int Flush();
  int Close();

  int fd() { return fd_; }

 protected:
  int fd_;
};

} /* namespace cbb */

#endif /* UTIL_FILE_CONTROL_H_ */
