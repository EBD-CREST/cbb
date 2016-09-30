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
#include <iostream>
#include <string>
#include <map>

#include <fuse.h>

#include "cbb/burst_buffer_client.h"
#include "cbb_client_wrapper.h"

// CBFSで処理を行うFUSEのラッパー関数

static std::string g_config_path;
static cbb::BurstBufferClient *g_client_ptr;

typedef std::map<uint64_t, cbb::File> FileTable;
static FileTable g_files;

/**
 * @breaf FUSEエラーチェック関数
 * @param error エラー値
 * @return エラー値
 */
static inline int cbb_to_fuse_error(cbb::Error error) {
#if 0
  if (error <= 0)
  return static_cast<int>(error);
  else
  return -EAGAIN;
#endif

  return static_cast<int>(error);
}



/// FUSE wrapper : getattr
int CBFSGetAttr(const char *path, struct stat *buf) {
  cbb::FileStat file_stat;
  cbb::Error error = g_client_ptr->GetAttr(path, &file_stat);
  if (error != cbb::kCBBSuccess)
    return cbb_to_fuse_error(error);

  buf->st_dev = file_stat.st_dev;
  buf->st_ino = file_stat.st_ino;
  buf->st_mode = file_stat.st_mode;
  buf->st_nlink = file_stat.st_nlink;
  buf->st_uid = file_stat.st_uid;
  buf->st_gid = file_stat.st_gid;
  buf->st_rdev = file_stat.st_rdev;
  buf->st_size = file_stat.st_size;
  buf->st_blksize = file_stat.st_blksize;
  buf->st_blocks = file_stat.st_blocks;

  buf->st_atim.tv_sec = file_stat.st_atim.tv_sec;
  buf->st_atim.tv_nsec = file_stat.st_atim.tv_nsec;

  buf->st_mtim.tv_sec = file_stat.st_mtim.tv_sec;
  buf->st_mtim.tv_nsec = file_stat.st_mtim.tv_nsec;

  buf->st_ctim.tv_sec = file_stat.st_ctim.tv_sec;
  buf->st_ctim.tv_nsec = file_stat.st_ctim.tv_nsec;

  return 0;
}

/// FUSE wrapper : readlink
int CBFSReadLink(const char *path, char *buf, size_t size) {
  return cbb_to_fuse_error(g_client_ptr->ReadLink(path, buf, size));
}

/// FUSE wrapper : mkdir
int CBFSMkDir(const char *path, mode_t mode) {
  return cbb_to_fuse_error(g_client_ptr->MkDir(path, mode));
}

/// FUSE wrapper : unlink
int CBFSUnlink(const char *path) {
  return cbb_to_fuse_error(g_client_ptr->Unlink(path));
}

/// FUSE wrapper : rmdir
int CBFSRmDir(const char *path) {
  return cbb_to_fuse_error(g_client_ptr->RmDir(path));
}

/// FUSE wrapper : symlink
int CBFSSymlink(const char *path, const char *link) {
  return cbb_to_fuse_error(g_client_ptr->Symlink(path, link));
}

/// FUSE wrapper : rename
int CBFSRename(const char *old_path, const char *new_path) {
  return cbb_to_fuse_error(g_client_ptr->Rename(old_path, new_path));
}

/// FUSE wrapper : link
int CBFSLink(const char *path, const char *newpath) {
  return cbb_to_fuse_error(g_client_ptr->Link(path, newpath));
}

/// FUSE wrapper : chmod
int CBFSChmod(const char *path, mode_t mode) {
  return cbb_to_fuse_error(g_client_ptr->Chmod(path, mode));
}

/// FUSE wrapper : chown
int CBFSChown(const char *path, uid_t uid, gid_t gid) {
  return cbb_to_fuse_error(g_client_ptr->Chown(path, uid, gid));
}

/// FUSE wrapper : truncate
int CBFSTruncate(const char *path, off_t size) {
  return cbb_to_fuse_error(g_client_ptr->Truncate(path, size));
}



/// FUSE wrapper : open
int CBFSOpen(const char *path, struct fuse_file_info *fi) {
  cbb::File file;
  cbb::Error error = g_client_ptr->Open(path, fi->flags, &file);
  if (error != cbb::kCBBSuccess)
    return cbb_to_fuse_error(error);

  fi->fh = file.fd;
  g_files[fi->fh] = file;

  return 0;
}

/// FUSE wrapper : read
int CBFSRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  cbb::File file = g_files[fi->fh];
  ssize_t ssize = 0;
  cbb::Error error = g_client_ptr->Read(file, buf, size, offset, &ssize);
  if (error != cbb::kCBBSuccess)
    return cbb_to_fuse_error(error);

  return ssize;
}

/// FUSE wrapper : write
int CBFSWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  cbb::File file = g_files[fi->fh];
  ssize_t ssize = 0;
  cbb::Error error = g_client_ptr->Write(file, buf, size, offset, &ssize);
  if (error != cbb::kCBBSuccess)
    return cbb_to_fuse_error(error);

  return ssize;
}

/// FUSE wrapper : statfs
int CBFSStatFs(const char *path, struct statvfs *buf) {
  return cbb_to_fuse_error(g_client_ptr->StatFs(path, buf));
}

/// FUSE wrapper : flush
int CBFSFlush(const char *path, struct fuse_file_info *fi) {
  cbb::File file = g_files[fi->fh];
  return cbb_to_fuse_error(g_client_ptr->Flush(file));
}

/// FUSE wrapper : release
int CBFSRelease(const char *path, struct fuse_file_info *fi) {
  cbb::File file = g_files[fi->fh];
  return cbb_to_fuse_error(g_client_ptr->Release(file));
}

/// FUSE wrapper : fsync
int CBFSFSync(const char *path, int datasync, struct fuse_file_info *fi) {
  cbb::File file = g_files[fi->fh];
  return cbb_to_fuse_error(g_client_ptr->FSync(file, datasync));
}



/// FUSE wrapper : setxattr
int CBFSSetXAttr(const char *path, const char *name, const char *value, size_t size, int flags) {
  return cbb_to_fuse_error(g_client_ptr->SetXAttr(path, name, value, size, flags));
}

/// FUSE wrapper : getxattr
int CBFSGetXAttr(const char *path, const char *name, char *value, size_t size) {
  return cbb_to_fuse_error(g_client_ptr->GetXAttr(path, name, value, size));
}

/// FUSE wrapper : listxattr
int CBFSListXAttr(const char *path, char *list, size_t size) {
  return cbb_to_fuse_error(g_client_ptr->ListXAttr(path, list, size));
}

/// FUSE wrapper : removexattr
int CBFSRemoveXAttr(const char *path, const char *name) {
  return cbb_to_fuse_error(g_client_ptr->RemoveXAttr(path, name));
}



/// FUSE wrapper : readdir
int CBFSReadDir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  cbb::FileStats file_stats;

  cbb::Error error = g_client_ptr->ReadDir(path, offset, &file_stats, cbb::kDirAll);
  if (error != cbb::kCBBSuccess)
    return cbb_to_fuse_error(error);

  for (cbb::FileStats::iterator it = file_stats.begin(); it != file_stats.end(); ++it) {
    struct stat st;
    memset(&st, 0, sizeof(st));

    std::string path = it->first;
    cbb::FileStat file_stat = it->second;

    st.st_ino = file_stat.st_ino;
    st.st_mode = file_stat.st_mode;

    filler(buf, path.c_str(), &st, 0);
  }

  return 0;
}

/// FUSE wrapper : fsyncdir
int CBFSFSyncDir(const char *path, int datasync, struct fuse_file_info *fi) {
  cbb::File file = g_files[fi->fh];
  return cbb_to_fuse_error(g_client_ptr->FSyncDir(path, datasync, file));
}



/// FUSE wrapper : init
void* CBFSInit(struct fuse_conn_info *conn) {
  g_client_ptr = new cbb::BurstBufferClient();
  assert(g_client_ptr != NULL);

  return (void*)g_client_ptr->Init(CBFSGetConfigPath());
}

/// FUSE wrapper : destroy
void CBFSDestroy(void *private_data) {
  delete g_client_ptr;
}



/// FUSE wrapper : access
int CBFSAccess(const char *path, int mode) {
  return cbb_to_fuse_error(g_client_ptr->Access(path, mode));
}

/// FUSE wrapper : create
int CBFSCreate(const char *path, mode_t mode, struct fuse_file_info *fi) {
  cbb::File file;
  cbb::Error error = g_client_ptr->Create(path, fi->flags, mode, &file);
  if (error != cbb::kCBBSuccess)
    return cbb_to_fuse_error(error);

  fi->fh = file.fd;
  g_files[fi->fh] = file;

  return 0;
}

/// FUSE wrapper : ftruncate
int CBFSFTruncate(const char *path, off_t size, struct fuse_file_info *fi) {
  cbb::File file = g_files[fi->fh];
  return cbb_to_fuse_error(g_client_ptr->FTruncate(file, size));
}

/// FUSE wrapper : fgetattr
int CBFSFGetAttr(const char *path, struct stat *statbuf, struct fuse_file_info *fi) {
  cbb::FileStat file_stat;
  cbb::File file = g_files[fi->fh];
  cbb::Error error = g_client_ptr->FGetAttr(path, &file_stat, file);
  if (error != cbb::kCBBSuccess)
    return cbb_to_fuse_error(error);

  return error;
}

/// FUSE wrapper : lock
int CBFSLock(const char *path, struct fuse_file_info *fi, int cmd, struct flock *lockbuf) {
  cbb::File file = g_files[fi->fh];
  cbb::Error error = g_client_ptr->Lock(path, file, cmd, lockbuf);
  if (error != cbb::kCBBSuccess)
    return cbb_to_fuse_error(error);

  return error;
}

/// FUSE wrapper : utimens
int CBFSUtimens(const char *path, const struct timespec ts[2]) {
  cbb::TimeSpec times[2];

  times[0].tv_sec = ts[0].tv_sec;
  times[0].tv_nsec = ts[0].tv_nsec;

  times[1].tv_sec = ts[1].tv_sec;
  times[1].tv_nsec = ts[1].tv_nsec;

  return cbb_to_fuse_error(g_client_ptr->Utimens(path, times));
}



/**
 * @breaf Config File Path 取得
 * @return ファイルパス
 */
const char *CBFSGetConfigPath() {
  return g_config_path.c_str();
}

/**
 * @breaf Config File Path 設定
 * @param filename Config File Path
 * @return なし
 */
void CBFSSetConfigPath(const char *filename) {
  g_config_path = filename;
}
