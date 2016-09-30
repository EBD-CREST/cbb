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
#include <sys/stat.h>
#include <sys/time.h>

#include "common/error.h"
#include "common/common.h"
#include "util/file_control.h"
#include "meta_data_manager.h"

// 各ファイル等のメタデータマネージャークラス
namespace cbb {

/**
 * @breaf テーブル登録
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param mutex_file Mutexファイル
 * @return Error値
 */
Error MetaDataManager::Register(const std::string &path, int fd) {
  table_[path].insert(fd);
  return kCBBSuccess;
}

/**
 * @breaf テーブル登録解除
 * @param path ファイルパス
 * @return Error値
 */
Error MetaDataManager::Unregister(const std::string &path) {
  BufferedFiles::iterator it = table_.find(path);
  if (it != table_.end()) {
    for (std::set<int>::iterator it_fd = it->second.begin(); it_fd != it->second.end(); it_fd++) {
      FileControl file_control(*it_fd);
      file_control.Close();
    }
    table_.erase(path);
  }

  return kCBBSuccess;
}

/**
 * @breaf テーブル登録解除
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @return Error値
 */
Error MetaDataManager::Unregister(const std::string &path, int fd) {
  BufferedFiles::iterator it = table_.find(path);
  if (it != table_.end()) {
    it->second.erase(fd);
    if (it->second.empty()) {
      table_.erase(path);
    }
  }
  return kCBBSuccess;
}

/**
 * @breaf ファイル属性取得
 * @param path ファイルパス
 * @param file_stat_ptr ファイル属性保存ポインタ
 * @param link_path リンクパス保存
 * @return Error値
 */
Error MetaDataManager::GetFileStat(const std::string &path, FileStat *file_stat_ptr, std::string &link_path) {

  FileStat file_stat;
  struct stat st;
  std::string tpath = target_path(path);
  boost::system::error_code ec;

  link_path = "";

  if (!boost::filesystem::exists(tpath, ec)) {
    return -ENOENT;
  }

  Error error = lstat(tpath.c_str(), &st);
  if (error != kCBBSuccess) {
    return errno_to_cbb_error(error);
  }

  file_stat.st_dev = st.st_dev;
  file_stat.st_ino = st.st_ino;
  file_stat.st_mode = st.st_mode;
  file_stat.st_nlink = st.st_nlink;
  file_stat.st_uid = st.st_uid;
  file_stat.st_gid = st.st_gid;
  file_stat.st_rdev = st.st_rdev;
  file_stat.st_size = st.st_size;
  file_stat.st_blksize = st.st_blksize;
  file_stat.st_blocks = st.st_blocks;

  file_stat.st_atim.tv_sec = st.st_atim.tv_sec;
  file_stat.st_atim.tv_nsec = st.st_atim.tv_nsec;

  file_stat.st_mtim.tv_sec = st.st_mtim.tv_sec;
  file_stat.st_mtim.tv_nsec = st.st_mtim.tv_nsec;

  file_stat.st_ctim.tv_sec = st.st_ctim.tv_sec;
  file_stat.st_ctim.tv_nsec = st.st_ctim.tv_nsec;

  // Virtual Symlinkの場合はリンク先を読み込んでそれを返す
  if (is_virtual_symlink(tpath)) {
    std::string line = read_one_line(tpath);
    if (!line.empty()) {
      link_path = line;
      file_stat.st_mode |= S_IFLNK; // symlink属性付加
    }
  }
  // Virtual Linkの場合はリンク先を読み込んでそれを返す
  else if (is_virtual_link(tpath)) {
    std::string line = read_one_line(tpath);
    if (!line.empty()) {
      link_path = line;
//      file_stat.st_mode |= S_IFLNK;
    }
  }

  *file_stat_ptr = file_stat;
  
  return error;
}

/**
 * @breaf ファイル属性取得 (FD版)
 * @param path ファイルパス
 * @param file_stat_ptr ファイル属性保存ポインタ
 * @return Error値
 */
Error MetaDataManager::GetFileStatFD(int fd, FileStat *file_stat_ptr) {

  FileStat file_stat;
  struct stat st;

  Error error = fstat(fd, &st);
  if (error != kCBBSuccess) {
    return errno_to_cbb_error(error);
  }

  file_stat.st_dev = st.st_dev;
  file_stat.st_ino = st.st_ino;
  file_stat.st_mode = st.st_mode;
  file_stat.st_nlink = st.st_nlink;
  file_stat.st_uid = st.st_uid;
  file_stat.st_gid = st.st_gid;
  file_stat.st_rdev = st.st_rdev;
  file_stat.st_size = st.st_size;
  file_stat.st_blksize = st.st_blksize;
  file_stat.st_blocks = st.st_blocks;

  file_stat.st_atim.tv_sec = st.st_atim.tv_sec;
  file_stat.st_atim.tv_nsec = st.st_atim.tv_nsec;

  file_stat.st_mtim.tv_sec = st.st_mtim.tv_sec;
  file_stat.st_mtim.tv_nsec = st.st_mtim.tv_nsec;

  file_stat.st_ctim.tv_sec = st.st_ctim.tv_sec;
  file_stat.st_ctim.tv_nsec = st.st_ctim.tv_nsec;

  *file_stat_ptr = file_stat;

  return error;
}

/**
 * @breaf リネーム
 * @param old_path 変更前ファイルパス
 * @param new_path 変更後ファイルパス
 * @return Error値
 */
Error MetaDataManager::Rename(const std::string &old_path, const std::string &new_path) {

  Error error = kCBBSuccess;

  if (exists_on_local(old_path)) {
    error = errno_to_cbb_error(rename(local_path(old_path).c_str(), local_path(new_path).c_str()));
    Unregister(old_path);
  }

  if (exists_on_secondary(old_path)) {
    errno_to_cbb_error(rename(secondary_path(old_path).c_str(), secondary_path(new_path).c_str()));
  }
  
  return error;
}

/**
 * @breaf ファイル属性変更
 * @param path ファイルパス
 * @param mode モード値
 * @return Error値
 */
Error MetaDataManager::Chmod(const std::string &path, mode_t mode) {
  Error error = kCBBSuccess;

  if (exists_on_local(path)) {
    error = errno_to_cbb_error(chmod(local_path(path).c_str(), mode));
  }

  if (exists_on_secondary(path)) {
    errno_to_cbb_error(chmod(secondary_path(path).c_str(), mode));
  }

  return error;
}

/**
 * @breaf ファイルオーナー変更
 * @param path ファイルパス
 * @param uid UID
 * @param gid GID
 * @return Error値
 */
Error MetaDataManager::Chown(const std::string &path, uid_t uid, gid_t gid) {
  Error error = kCBBSuccess;

  if (exists_on_local(path)) {
    error = errno_to_cbb_error(lchown(local_path(path).c_str(), uid, gid));
  }

  if (exists_on_secondary(path)) {
    errno_to_cbb_error(lchown(secondary_path(path).c_str(), uid, gid));
  }

  return error;
}

/**
 * @breaf ファイルサイズ変更
 * @param path ファイルパス
 * @param size サイズ
 * @return Error値
 */
Error MetaDataManager::Truncate(const std::string &path, off_t size) {
  return errno_to_cbb_error(truncate(target_path(path).c_str(), size));
}

/**
 * @breaf ファイルサイズ変更
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param size サイズ
 * @return Error値
 */
Error MetaDataManager::FTruncate(const std::string &path, int fd, off_t size) {
  return errno_to_cbb_error(ftruncate(fd, size));
}

/**
 * @breaf ファイルアクセス
 * @param path ファイルパス
 * @param mode モード値
 * @return Error値
 */
Error MetaDataManager::Access(const std::string &path, mode_t mode) {
  return errno_to_cbb_error(access(target_path(path).c_str(), mode));
}

/**
 * @breaf ファイル日時変更
 * @param path ファイルパス
 * @param time0 日時情報
 * @param time1 日時情報
 * @return Error値
 */
Error MetaDataManager::Utimens(const std::string &path, const TimeSpec &time0, const TimeSpec &time1) {
  struct timeval times[2];

  times[0].tv_sec = time0.tv_sec;
  times[0].tv_usec = time0.tv_nsec / 1000;
  times[1].tv_sec = time1.tv_sec;
  times[1].tv_usec = time1.tv_nsec / 1000;

  return errno_to_cbb_error(utimes(target_path(path).c_str(), times));
}

/**
 * @breaf ファイル作成
 * @param path ファイルパス
 * @param flags フラグ
 * @param mode モード値
 * @return Error値
 */
Error MetaDataManager::Create(const std::string &path, int flags, mode_t mode) {
  FileControl file_control;
  int fd = file_control.Create(local_path(path).c_str(), flags, mode);

  if (fd == -1) {
    fd = -errno;
  } else {
    Register(path, fd);
  }

  return fd;
}

/**
 * @breaf ファイルオープン
 * @param path ファイルパス
 * @param flags フラグ
 * @return Error値
 */
Error MetaDataManager::Open(const std::string &path, int flags) {
  FileControl file_control;
  int fd = 0;

  if (exists_on_secondary(path) && !exists_on_local(path)) {
    CopySecondaryToLocal(path);
  }

  fd = file_control.Open(local_path(path).c_str(), flags);
  if (fd == -1) {
    fd = errno_to_cbb_error(fd);
  } else {
    Register(path, fd);
  }

  return fd;
}

/**
 * @breaf ファイル読み込み
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param buf バッファ
 * @param size サイズ
 * @param offset オフセット
 * @return Error値
 */
Error MetaDataManager::Read(const std::string &path, int fd, void *buf, size_t size, off_t offset) {
  FileControl file_control(fd);
  return file_control.Read(buf, size, offset);
}

/**
 * @breaf ファイル書き込み
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param buf バッファ
 * @param size バッファサイズ
 * @param offset オフセット
 * @return Error値
 */
Error MetaDataManager::Write(const std::string &path, int fd, const void *buf, size_t size, off_t offset) {
  FileControl file_control(fd);
  return file_control.Write(buf, size, offset);
}

/**
 * @breaf ファイル同期
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param is_data データかどうか
 * @return Error値
 */
Error MetaDataManager::FSync(const std::string &path, int fd, int is_data) {
  FileControl file_control(fd);
  return file_control.FSync(is_data);
}

/**
 * @breaf ファイルフラッシュ
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @return Error値
 */
Error MetaDataManager::Flush(const std::string &path, int fd) {
  FileControl file_control(fd);
  return file_control.Flush();
}

/**
 * @breaf ファイル開放（クローズ）
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @return Error値
 */
Error MetaDataManager::Close(const std::string &path, int fd) {
  FileControl file_control(fd);
  Error ret = file_control.Close();
  Unregister(path, fd);
  return ret;
}

/**
 * @breaf SecondaryからLocalにファイルをコピー
 * @param path ファイルパス
 * @return Error値
 */
Error MetaDataManager::CopySecondaryToLocal(const std::string &path) {
  Error ret = -1;
  bool is_copy = true;

  std::string source = secondary_path(path);
  std::string destination = local_path(path);

DMSG("CopySecondaryToLocal src: %s >>> dst: %s : path = %s \n", source.c_str(), destination.c_str(), path.c_str());

  if (boost::filesystem::exists(source)) {
    if (boost::filesystem::exists(destination)) {
      const std::time_t last_update_src = boost::filesystem::last_write_time(source);
      const std::time_t last_update_dst = boost::filesystem::last_write_time(destination);

      if (last_update_src <= last_update_dst) {
        is_copy = false;
      }
    }

    if (is_copy) {
      boost::system::error_code ec;
      boost::filesystem::copy_file(source, destination, boost::filesystem::copy_option::overwrite_if_exists, ec);
    }
    ret = 0;
  } else {
    DMSG(">>> not exists : %s \n", path.c_str());
    ret = 0;
  }

  return ret;
}

/**
 * @breaf ファイルのフラッシュ（クローズ）
 * @param path ファイルパス
 * @return Error値
 */
Error MetaDataManager::FileFlush(const std::string &path) {
  return Unregister(path);
}

} // namespace 
