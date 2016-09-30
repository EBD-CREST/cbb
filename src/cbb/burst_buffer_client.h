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
#ifndef CBB_BURST_BUFFER_CLIENT_H_
#define CBB_BURST_BUFFER_CLIENT_H_

#include <sys/types.h>
#include <stdint.h>

#include <string>
#include <list>

#include <jubatus/msgpack/rpc/client.h>
#include <jubatus/msgpack/rpc/session_pool.h>

#include "common/error.h"
#include "common/common.h"
#include "util/settings.h"
#include "util/select_server.h"
#include "util/thread.h"

namespace cbb {

struct File {
  std::string path;
  std::string bb_host;
  uint16_t bb_port;
  uint64_t fd_org;
  uint64_t fd;
};

struct FileStat;
struct TimeVal;

// CBFSモジュール（クライアント側）のメイン処理クラス
class BurstBufferClient : public Thread {

 public:

  BurstBufferClient();
  virtual ~BurstBufferClient();

  Error GetAttr(const char *path, FileStat *file_stat_ptr);
  Error GetAttrEx(const char *path, FileStat *file_stat_ptr, std::string &link_path);
  Error ReadLink(const char *path, char *buf, size_t size);
  Error MkDir(const char *path, mode_t mode);
  Error Unlink(const char *path);
  Error RmDir(const char *path);
  Error Symlink(const char *path, const char *link); //*
  Error Rename(const char *old_path, const char *new_path);
  Error Link(const char *path, const char *newpath); //*
  Error Chmod(const char *path, mode_t mode);
  Error Chown(const char *path, uid_t uid, gid_t gid);
  Error Truncate(const char *path, off_t size);

  Error Open(const char *path, int flags, File *file_ptr);  
  Error Read(const File &file, char *buf, size_t size, off_t offset, ssize_t *ssize_ptr);
  Error Write(const File &file, const char *buf, size_t size, off_t pos, ssize_t *ssize_ptr);
  Error StatFs(const char *path, struct statvfs *buf); //*
  Error Flush(const File &file);
  Error Release(const File &file);
  Error FSync(const File &file, int datasync);

  Error ReadDir(const char *path, off_t offset, FileStats *file_stats_ptr, int type);
  Error FSyncDir(const char *path, int datasync, const File &file); //*

  Error SetXAttr(const char *path, const char *name, const char *value, size_t size, int flags); //*
  Error GetXAttr(const char *path, const char *name, char *value, size_t size); //*
  Error ListXAttr(const char *path, char *list, size_t size); //*
  Error RemoveXAttr(const char *path, const char *name); //*

  Error Init(const char *config_path);
  Error Destroy();

  Error Access(const char *path, int mode);
  Error Create(const char *path, int flags,  mode_t mode, File *file_ptr);
  Error FTruncate(const File &file, off_t size);
  Error FGetAttr(const char *path, FileStat *file_stat_ptr, const File &file); //*
  Error Lock(const char *path, const File &file, int cmd, struct flock *lockbuf); //*
  Error Utimens(const char *path, const TimeSpec times[2]);


  Error FilePrevRead(const char *path);
  Error FileFlush(const char *path);
  Error LocalFileExport();


 protected:
  bool ThreadCall(void *user_data);

 private:

  Error GetBurstBuffer(const char *path, std::string *bb_host_ptr, uint16_t *bb_port_ptr);

  Error GetAttrExInternal(const char *path, FileStat *file_stat_ptr, std::string &link_path);
  Error UnlinkInternal(const char *path, bool is_all_server);

  void StartPrevFileRead(const char* path);

  msgpack::rpc::session_pool session_pool_;
  msgpack::rpc::shared_zone life_;

  Settings settings_;
  SelectServer select_server_;
  std::string prev_read_dir_;
  std::string target_filename_;
};

} // namespace cbb

#endif // CBB_BURST_BUFFER_CLIENT_H_
