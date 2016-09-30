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
#ifndef CBFS_CBB_CLIENT_WRAPPER_H_
#define CBFS_CBB_CLIENT_WRAPPER_H_

// CBFSで処理を行うFUSEのラッパー関数

#ifdef __cplusplus
extern "C" {
#endif

  int CBFSGetAttr(const char *path, struct stat *statbuf);
  int CBFSReadLink(const char *path, char *link, size_t size);
  int CBFSMkDir(const char *path, mode_t mode);
  int CBFSUnlink(const char *path);
  int CBFSRmDir(const char *path);
  int CBFSSymlink(const char *path, const char *link); //*
  int CBFSRename(const char *old_path, const char *new_path);
  int CBFSLink(const char *path, const char *newpath); //*
  int CBFSChmod(const char *path, mode_t mode);
  int CBFSChown(const char *path, uid_t uid, gid_t gid);
  int CBFSTruncate(const char *path, off_t size);

  int CBFSOpen(const char *path, struct fuse_file_info *fi);
  int CBFSRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
  int CBFSWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
  int CBFSStatFs(const char *path, struct statvfs *statInfo); //*
  int CBFSFlush(const char *path, struct fuse_file_info *fi);
  int CBFSRelease(const char *path, struct fuse_file_info *fi);
  int CBFSFSync(const char *path, int datasync, struct fuse_file_info *fi);

  int CBFSSetXAttr(const char *path, const char *name, const char *value, size_t size, int flags); //*
  int CBFSGetXAttr(const char *path, const char *name, char *value, size_t size); //*
  int CBFSListXAttr(const char *path, char *list, size_t size); //*
  int CBFSRemoveXAttr(const char *path, const char *name); //*

  int CBFSReadDir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
  int CBFSFSyncDir(const char *path, int datasync, struct fuse_file_info *fi); //*

  void *CBFSInit(struct fuse_conn_info *conn);
  void CBFSDestroy(void *private_data);

  int CBFSAccess(const char *path, int mode);
  int CBFSCreate(const char *path, mode_t mode, struct fuse_file_info *fi);
  int CBFSFTruncate(const char *path, off_t size, struct fuse_file_info *fi);
  int CBFSFGetAttr(const char *path, struct stat *statbuf, struct fuse_file_info *fi); //*
  int CBFSLock(const char *path, struct fuse_file_info *fi, int cmd, struct flock *lockbuf); //*
  int CBFSUtimens(const char *path, const struct timespec ts[2]);

  const char *CBFSGetConfigPath();
  void CBFSSetConfigPath(const char *filename);

#ifdef __cplusplus
}
#endif

#endif // CBFS_CBB_CLIENT_WRAPPER_H_
