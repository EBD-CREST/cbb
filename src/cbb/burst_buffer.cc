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
#include <dirent.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <sys/file.h>

#include <cstdio>
#include <iostream>
#include <cerrno>
#include <cassert>

#include <boost/filesystem.hpp>

#include "common/error.h"
#include "common/common.h"
#include "meta_data_manager.h"
#include "burst_buffer.h"
#include "util/options.h"
#include "util/settings.h"

// CBBモジュール（サーバー側）のメイン処理クラス
namespace cbb {

/**
 * @breaf Constractor
 * @param local_storage_root_path ローカルストレージルートパス
 * @param secondary_storage_root_path セカンダリーストレージパス
 * @param interval_time ファイル監視時間間隔 (min)
 */
BurstBuffer::BurstBuffer(std::string local_storage_root_path, std::string secondary_storage_root_path, int interval_time)
    : md_manager_(local_storage_root_path, secondary_storage_root_path) {
  DuplicateDirSecondaryToLocal(secondary_storage_root_path);
  lf_exporter_.Create(&md_manager_, interval_time * 60 * 1000);
}

/**
 * @breaf destructor
 */
BurstBuffer::~BurstBuffer() {
  DMSG("destructor : BurstBuffer::~BurstBuffer \n");
  lf_exporter_.Release();
}

/**
 * @breaf ファイル属性取得
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 */
void BurstBuffer::GetAttr(msgpack::rpc::request req, const std::string &path) {
  DMSG("[GetAttr] : %s \n", path.c_str());

  FileStat file_stat;
  std::string link_path;
  Error error = md_manager_.GetFileStat(path, &file_stat, link_path);
  req.result(msgpack::type::make_tuple<Error, FileStat, std::string>(error, file_stat, link_path));
}

/**
 * @breaf リンク情報取得
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param buf バッファポインタ
 * @param size バッファサイズ
 */
void BurstBuffer::ReadLink(msgpack::rpc::request req, const std::string &path, size_t size) {
  DMSG("[ReadLink] : %s \n", path.c_str());

  msgpack::rpc::auto_zone life(new msgpack::zone());

  char *ptr = (char*)life->malloc(size);
  assert(ptr != NULL);
  memset(ptr, 0x00, size);
  
  ssize_t ssize = readlink(md_manager_.target_path(path).c_str(), ptr, size - 1);
  if (ssize == -1) {
    ssize = errno_to_cbb_error(ssize);

  } else {
    std::string target = md_manager_.remove_storage_path(ptr);
    memset(ptr, 0x00, size);
    strcpy(ptr, target.c_str());
    ssize = strlen(ptr) + 1;
  }

  msgpack::type::raw_ref buf(ptr, size);
  req.result(msgpack::type::make_tuple<Error, msgpack::type::raw_ref>(ssize, buf), life);
}

/**
 * @breaf ディレクトリ作成
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param mode モード値
 */
void BurstBuffer::MkDir(msgpack::rpc::request req, const std::string &path, mode_t mode) {
  DMSG("[MkDir] : %s %08lx \n", path.c_str(), mode);

  Error error = kCBBSuccess;
  boost::system::error_code ec;

  std::string target_path = md_manager_.local_path(path);
  if (!boost::filesystem::exists(target_path, ec)) {
    error = errno_to_cbb_error(mkdir(target_path.c_str(), mode));
  }

  target_path = md_manager_.secondary_path(path);
  if (!boost::filesystem::exists(target_path, ec)) {
    errno_to_cbb_error(mkdir(target_path.c_str(), mode));
  }

  req.result(error);
}

/**
 * @breaf ファイル削除
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 */
void BurstBuffer::Unlink(msgpack::rpc::request req, const std::string &path) {
  DMSG("[Unlink] : %s \n", path.c_str());

  Error error = kCBBSuccess;
  boost::system::error_code ec;

  std::string target_path = md_manager_.local_path(path);
  if (boost::filesystem::exists(target_path, ec)) {
    lf_exporter_.Unregister(path);
    md_manager_.Unregister(path);
    error = errno_to_cbb_error(unlink(target_path.c_str()));
  }

  target_path = md_manager_.secondary_path(path);
  if (boost::filesystem::exists(target_path, ec)) {
    errno_to_cbb_error(unlink(target_path.c_str()));
  }

  req.result(error);
}

/**
 * @breaf ディレクトリ削除
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 */
void BurstBuffer::RmDir(msgpack::rpc::request req, const std::string &path) {
  DMSG("[RmDir] : %s \n", path.c_str());

  std::string target_path = md_manager_.local_path(path);
  Error error = errno_to_cbb_error(rmdir(target_path.c_str()));

  target_path = md_manager_.secondary_path(path);
  if (boost::filesystem::exists(target_path)) {
    errno_to_cbb_error(rmdir(target_path.c_str()));
  }

  req.result(error);
}

/**
 * @breaf シンボリックリンク
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param link リンクパス
 */
void BurstBuffer::Symlink(msgpack::rpc::request req, const std::string &path, const std::string &link) {
  DMSG("[Symlink] : %s -> %s \n", path.c_str(), link.c_str());

  Error error = kCBBSuccess;

  // virtual symlink
  if (is_virtual_symlink(link)) {
    // 仮想的なシンボリックファイルを作成する
    write_one_line(md_manager_.local_path(link), path);

  } else {
    // ディレクトリの場合
    // local storage
    std::string target = md_manager_.local_path(path);
    if (boost::filesystem::is_directory(target)) {
      error = symlink(target.c_str(), md_manager_.local_path(link).c_str());
    }

    // secondary storage
    target = md_manager_.secondary_path(path);
    if (boost::filesystem::is_directory(target)) {
      symlink(target.c_str(), md_manager_.secondary_path(link).c_str());
    }
  }

  req.result(error);
}

/**
 * @breaf リネーム
 * @param req MsgPackリクエストオブジェクト
 * @param old_path 変更前ファイルパス
 * @param new_path 変更後ファイルパス
 */
void BurstBuffer::Rename(msgpack::rpc::request req, const std::string &old_path, const std::string &new_path) {
  DMSG("[Rename] : %s -> %s \n", old_path.c_str(), new_path.c_str());
  namespace fs = boost::filesystem;
  boost::system::error_code ec;
  Error error = kCBBSuccess;

  // ディレクトリの場合
  std::string target = md_manager_.local_path(old_path);
  if (fs::exists(target) && fs::is_directory(target)) {
DMSG("directory : %s \n", target.c_str());
    lf_exporter_.UnregisterAll();

    // Secondaryに新しい名前のディレクトリがある場合
    bool is_rename = true;
    std::string second = md_manager_.secondary_path(new_path);
    if (fs::exists(second) && fs::is_directory(second)) {
DMSG("dir secondary : %s \n", target.c_str());
      is_rename = false;
    } else {
      second = md_manager_.secondary_path(old_path);
    }

    // すべてのLocalファイルをSecondaryへコピーする
    fs::recursive_directory_iterator last;
    std::string local = target + "/";
    second = second + "/";
    for (fs::recursive_directory_iterator it(target); it != last; ++it) {
      if (fs::is_directory(it->path())) {
        std::string filename = std::string(it->path().filename().c_str()) + "/";
        local += filename;
        second += filename;
      } else {
        std::string filename = it->path().filename().c_str();
        if (fs::exists(local + filename)) {
          fs::copy_file(local + filename, second + filename, boost::filesystem::copy_option::overwrite_if_exists);
        }
      }
    }

    // Localのディレクトリを削除
    fs::remove_all(target);

    if (is_rename) {
      // Secondaryの名前を変更
      fs::rename(md_manager_.secondary_path(old_path), md_manager_.secondary_path(new_path), ec);
      if (ec) {
        error = -1;
      }
    }

    // Secondaryのディレクトリ階層をLocalへコピー
    local = new_path + "/";
    fs::create_directories(md_manager_.local_path(local));
    for (fs::recursive_directory_iterator it(md_manager_.secondary_path(new_path)); it != last; ++it) {
      if (fs::is_directory(it->path())) {
        std::string filename = std::string(it->path().filename().c_str()) + "/";
        local += filename;
        fs::create_directories(md_manager_.local_path(local));
      }
    }

    lf_exporter_.ReSearchLocalFiles();
  }
  // ファイルの場合
  else {
DMSG("file : %s \n", target.c_str());

    // Localにファイルがある場合
    if (fs::exists(target) && fs::is_regular_file(target)) {
DMSG("file : %s -> %s \n", target.c_str(), md_manager_.secondary_path(new_path).c_str());
      lf_exporter_.Unregister(old_path);
      fs::remove(md_manager_.secondary_path(old_path), ec);

//      fs::rename(target, md_manager_.secondary_path(new_path), ec);
      fs::copy_file(target, md_manager_.secondary_path(new_path), ec);
      if (ec) { error = -1; }
      fs::remove(target, ec);
    } else {
      // Secondaryにファイルがある場合
      target = md_manager_.secondary_path(old_path);
      if (fs::exists(target) && fs::is_regular_file(target)) {
DMSG("file : %s -> %s \n", target.c_str(), md_manager_.secondary_path(new_path).c_str());
        fs::rename(target, md_manager_.secondary_path(new_path), ec);
        if (ec) { error = -1; }
      }
    }
  }

  req.result(error);
}

/**
 * @breaf リンク
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param newpath 新ファイルパス
 */
void BurstBuffer::Link(msgpack::rpc::request req, const std::string &path, const std::string &newpath) {
  DMSG("[Link] : %s -> %s \n", path.c_str(), newpath.c_str());
  Error error = kCBBSuccess;

  // virtual link
  if (is_virtual_link(newpath)) {
    // 仮想的なリンクファイルを作成する
    write_one_line(md_manager_.local_path(newpath), path);

  } else {
    // ディレクトリの場合
    // local storage
    std::string target = md_manager_.local_path(path);
    if (boost::filesystem::is_directory(target)) {
      error = link(target.c_str(), md_manager_.local_path(newpath).c_str());
    }

    // secondary storage
    target = md_manager_.secondary_path(path);
    if (boost::filesystem::is_directory(target)) {
      // セカンドにシンボリックリンクを作る
      error = link(target.c_str(), md_manager_.secondary_path(newpath).c_str());
    }
  }

  req.result(error);
}

/**
 * @breaf ファイル属性変更
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param mode モード値
 */
void BurstBuffer::Chmod(msgpack::rpc::request req, const std::string &path, mode_t mode) {
  DMSG("[Chmod] : %s %08lx \n", path.c_str(), mode);

  req.result(md_manager_.Chmod(path, mode));
}

/**
 * @breaf ファイルオーナー変更
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param uid UID
 * @param gid GID
 */
void BurstBuffer::Chown(msgpack::rpc::request req, const std::string &path, uid_t uid, gid_t gid) {
  DMSG("[Chown] : %s %08lx %08lx \n", path.c_str(), uid, gid);

  req.result(md_manager_.Chown(path, uid, gid));
}

/**
 * @breaf ファイルサイズ変更
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param size サイズ
 */
void BurstBuffer::Truncate(msgpack::rpc::request req, const std::string &path, off_t size) {
  DMSG("[Truncate] : %s \n", path.c_str());

  req.result(md_manager_.Truncate(path, size));
}



/**
 * @breaf ファイルオープン
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param flags フラグ
 */
void BurstBuffer::Open(msgpack::rpc::request req, const std::string &path, int flags) {
  // 書き込み属性がある場合
  if (flags & (O_WRONLY | O_RDWR) != 0) {
    lf_exporter_.Unregister(path);
  }

  int fd = md_manager_.Open(path, flags);

  DMSG("[Open] : %s %08lx  fd:%d \n", path.c_str(), flags, fd);

  req.result(fd);
}

/**
 * @breaf ファイル読み込み
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param size サイズ
 * @param offset オフセット
 */
void BurstBuffer::Read(msgpack::rpc::request req, const std::string &path, int fd, size_t size, off_t offset) {
  msgpack::rpc::auto_zone life(new msgpack::zone());

  char *ptr = (char*)life->malloc(size);
  assert(ptr != NULL);
  
  ssize_t ssize = md_manager_.Read(path, fd, ptr, size, offset);

  DMSG("[Read] : %s  fd:%d  off:%d  size:%d -> size:%d\n", path.c_str(), fd, offset, size, ssize);

  msgpack::type::raw_ref buf(ptr, size);
  req.result(msgpack::type::make_tuple<ssize_t, msgpack::type::raw_ref>(ssize, buf), life);
}

/**
 * @breaf ファイル書き込み
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param offset オフセット
 * @param raw MsgPack Rawデータ情報
 */
void BurstBuffer::Write(msgpack::rpc::request req, const std::string &path, int fd, off_t offset, const msgpack::type::raw_ref &raw) {
  //std::cout << "[WRITE] " << md_manager_.secondary_path(path) <<  " fd: " << fd << std::endl;
  
  ssize_t ssize = md_manager_.Write(path, fd, raw.ptr, raw.size, offset);

  DMSG("[Write] : %s  fd:%d  off:%d  size:%d -> size:%d\n", path.c_str(), fd, offset, raw.size, ssize);

  req.result(ssize);
}

/**
 *
 */
void BurstBuffer::StatFs(msgpack::rpc::request req, const std::string &path) {
  DMSG("[StatFs] : %s \n", path.c_str());

  std::string target = md_manager_.target_path(path);
  struct StatVfs stat_info;

  Error error = statvfs(target.c_str(), &stat_info);

  req.result(msgpack::type::make_tuple<Error, StatVfs>(error, stat_info));
}

/**
 * @breaf ファイルフラッシュ
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 */
void BurstBuffer::Flush(msgpack::rpc::request req, const std::string &path, int fd) {
  DMSG("[Flush] : %s  fd:%d \n", path.c_str(), fd);

  req.result(md_manager_.Flush(path, fd));
}

/**
 * @breaf ファイル開放（クローズ）
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 */
void BurstBuffer::Release(msgpack::rpc::request req, const std::string &path, int fd) {
  DMSG("[Release] : %s  fd:%d \n", path.c_str(), fd);

  req.result(md_manager_.Close(path, fd));
  lf_exporter_.Register(path);
}

/**
 * @breaf ファイル同期
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param datasync データ同期
 */
void BurstBuffer::FSync(msgpack::rpc::request req, const std::string &path, int fd, int datasync) {
  DMSG("[FSync] : %s  fd:%d \n", path.c_str(), fd);

  req.result(md_manager_.FSync(path, fd, datasync));
}



/**
 * @breaf ディレクトリリスト読み込み補助関数
 * @param path ファイルパス
 * @param offset オフセット
 * @param file_stats_ptr ファイル情報配列ポインタ
 * @return Error値
 */
Error BurstBuffer::ReadDirInternal(const std::string &path, off_t offset, FileStats &file_stats) {
  Error error = kCBBSuccess;
  DIR *dp = opendir(path.c_str());
  if (dp == NULL) {
    error = errno_to_cbb_error(-1);
    return error;
  }

  seekdir(dp, offset);

  struct dirent *de;
  while ((de = readdir(dp)) != NULL) {
    FileStat file_stat;
    std::string d_name = std::string(de->d_name);

    // virtual symlink check
    if (is_virtual_symlink(d_name)) {
      d_name = remove_virtual_ext(d_name);
      de->d_type = de->d_type | DT_LNK;
    }
    // virtual link check
    else if (is_virtual_link(d_name)) {
      d_name = remove_virtual_ext(d_name);
//      de->d_type = de->d_type | DT_LNK;
    }

    file_stat.st_ino = de->d_ino;
    file_stat.st_mode = de->d_type << 12;

    if (file_stats.find(d_name) == file_stats.end()) {
      file_stats[d_name] = file_stat;
    }
  }

  error = closedir(dp);
  if (error != kCBBSuccess) {
    errno_to_cbb_error(error);
    file_stats.clear();
  }
  
  return error;
}

/**
 * @breaf ディレクトリリスト読み込み
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param offset オフセット
 * @param type 読み取りディレクトリタイプ
 */
void BurstBuffer::ReadDir(msgpack::rpc::request req, const std::string &path, off_t offset, int type) {
  DMSG("[ReadDir] : %s \n", path.c_str());

  FileStats file_stats;
  Error error = kCBBSuccess;
/*
  // The following operation should be conducted in OpenDir() operation.
  struct stat st;
  if (!md_manager_.is_buffered(path) && lstat(md_manager_.secondary_path(path).c_str(), &st) == 0) {
    if (!boost::filesystem::exists(md_manager_.local_path(path))) {
      error = errno_to_cbb_error(mkdir(md_manager_.local_path(path).c_str(), st.st_mode));
    }
  }
*/

  if (type == kDirAll || type == kDirLocal) {
    error = ReadDirInternal(md_manager_.local_path(path), offset, file_stats);
    if (error != kCBBSuccess) {
      errno_to_cbb_error(error);
    }
  }

  if (type == kDirAll || type == kDirSecondary) {
    error = ReadDirInternal(md_manager_.secondary_path(path), offset, file_stats);
    if (error != kCBBSuccess) {
      errno_to_cbb_error(error);
    }
  }

  DMSG("[ReadDir] : count = %d\n", file_stats.size());

  req.result(msgpack::type::make_tuple<int, FileStats>(error, file_stats));
}

/**
 * @breaf ディレクトリ同期
 * @param req MsgPackリクエストオブジェクト
 * @param path ディレクトリパス
 * @param datasync データ同期
 */
void BurstBuffer::FSyncDir(msgpack::rpc::request req, const std::string &path, int datasync) {
  DMSG("[FSyncDir] : %s %d \n", path.c_str(), datasync);

  sync();
  sync();

  req.result(0);
}


/**
 * @breaf 拡張ファイル属性設定
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param name 名前
 * @param value 設定値
 * @param size サイズ
 * @param flags フラグ
 */
void BurstBuffer::SetXAttr(msgpack::rpc::request req, const std::string &path, const std::string &name, const std::string &value, size_t size, int flags) {
  DMSG("[SetXAttr] : %s %s %s %d %d \n", path.c_str(), name.c_str(), value.c_str(), size, flags);

  Error error = 0;
  boost::system::error_code ec;

  std::string target_path = md_manager_.local_path(path);
  if (!boost::filesystem::exists(target_path, ec)) {
    error = errno_to_cbb_error(lsetxattr(target_path.c_str(), name.c_str(), value.c_str(), size, flags));
  }

  target_path = md_manager_.secondary_path(path);
  if (!boost::filesystem::exists(target_path, ec)) {
    errno_to_cbb_error(lsetxattr(target_path.c_str(), name.c_str(), value.c_str(), size, flags));
  }

  req.result(error);
}

/**
 * @breaf 拡張ファイル属性取得
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param name 名前
 * @param value 設定値
 * @param size サイズ
 */
void BurstBuffer::GetXAttr(msgpack::rpc::request req, const std::string &path, const std::string &name, const std::string &value, size_t size) {
  DMSG("[GetXAttr] : %s %s %s %d \n", path.c_str(), name.c_str(), value.c_str(), size);

  std::string target = md_manager_.target_path(path);
  char *buf = (char *)malloc(size+1);
  memset(buf, 0x00, size+1);

  size_t ret = lgetxattr(target.c_str(), name.c_str(), buf, size);

  req.result(msgpack::type::make_tuple<size_t, std::string>(ret, std::string(buf)));
  free(buf);
}

/**
 * @breaf リスト属性
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param list リスト
 * @param size サイズ
 */
void BurstBuffer::ListXAttr(msgpack::rpc::request req, const std::string &path, const std::string &list, size_t size) {
  DMSG("[ListXAttr] : %s %s %d \n", path.c_str(), list.c_str(), size);

  std::string target = md_manager_.target_path(path);
  char *buf = (char *)malloc(size+1);
  memset(buf, 0x00, size+1);

  size_t ret = llistxattr(target.c_str(), buf, size);

  req.result(msgpack::type::make_tuple<size_t, std::string>(ret, std::string(buf)));
  free(buf);
}

/**
 * @breaf 拡張ファイル属性削除
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param name 名前
 */
void BurstBuffer::RemoveXAttr(msgpack::rpc::request req, const std::string &path, const std::string &name) {
  DMSG("[SetXAttr] : %s %s \n", path.c_str(), name.c_str());

  std::string target = md_manager_.target_path(path);
  Error error = lremovexattr(target.c_str(), name.c_str());

  req.result(error);
}



/**
 * @breaf ファイルアクセス
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param mode モード値
 */
void BurstBuffer::Access(msgpack::rpc::request req, const std::string &path, int mode) {
  DMSG("[Access] : %s %08lx \n", path.c_str(), mode);

  req.result(md_manager_.Access(path, mode));
}

/**
 * @breaf ファイル作成
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param flags フラグ
 * @param mode モード値
 */
void BurstBuffer::Create(msgpack::rpc::request req, const std::string &path, int flags, mode_t mode) {

  lf_exporter_.Unregister(path);
  int fd = md_manager_.Create(path, flags, mode);

  DMSG("[Create] : %s %08lx  fd:%d \n", path.c_str(), mode, fd);

  req.result(fd);
}

/**
 * @breaf ファイルサイズ変更
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param size サイズ
 */
void BurstBuffer::FTruncate(msgpack::rpc::request req, const std::string &path, int fd, off_t size) {
  DMSG("[FTruncate] : %s  fd:%d \n", path.c_str(), fd);

  req.result(md_manager_.FTruncate(path, fd, size));
}

/**
 * @breaf ファイル属性取得 (ファイルディスクリプタ版)
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 */
void BurstBuffer::FGetAttr(msgpack::rpc::request req, const std::string &path, int fd) {
  DMSG("[FGetAttr] : %s %d \n", path.c_str(), fd);

  FileStat file_stat;
  Error error = md_manager_.GetFileStatFD(fd, &file_stat);
  req.result(msgpack::type::make_tuple<Error, FileStat>(error, file_stat));
}

/**
 * @breaf ファイルロック
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param file ファイル情報
 * @param cmd コマンド
 */
void BurstBuffer::Lock(msgpack::rpc::request req, const std::string &path, int fd, int cmd) {
  DMSG("[Lock] : %s %d %d \n", path.c_str(), fd, cmd);

  Error error = flock(fd, cmd);

  req.result(error);
}

/**
 * @breaf ファイル日時変更
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 * @param time0 日時情報
 * @param time1 日時情報
 */
void BurstBuffer::Utimens(msgpack::rpc::request req, const std::string &path, const TimeSpec &time0, const TimeSpec &time1) {
  DMSG("[Utimens] : %s \n", path.c_str());

  req.result(md_manager_.Utimens(path, time0, time1));
}



/**
 * @breaf ファイル先読み
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 */
void BurstBuffer::FilePrevRead(msgpack::rpc::request req, const std::string &path) {
  DMSG("[FilePrevRead] : %s \n", path.c_str());

  Error error = md_manager_.CopySecondaryToLocal(path);

  if (error != 0) {
    DMSG("[FilePrevRead] : error = %d \n", error);
  }

  req.result(error);
}

/**
 * @breaf ファイルフラッシュ
 * @param req MsgPackリクエストオブジェクト
 * @param path ファイルパス
 */
void BurstBuffer::FileFlush(msgpack::rpc::request req, const std::string &path) {
  DMSG("[FileFlush] : %s \n", path.c_str());

  Error error = md_manager_.FileFlush(path);

  req.result(error);
}

/**
 * @breaf ローカルファイル書き出し
 * @param req MsgPackリクエストオブジェクト
 */
void BurstBuffer::LocalFileExport(msgpack::rpc::request req) {
  DMSG("[LocalFileExport] \n");

  lf_exporter_.CheckLocalFiles();

  req.result((Error)kCBBSuccess);
}


/**
 * @breaf MsgPack処理振り分け
 * @param req MsgPackリクエストオブジェクト
 */
void BurstBuffer::dispatch(msgpack::rpc::request req) {

  try {

    std::string method;
    req.method().convert(&method);

    DMSG("[dispatch] : %s \n", method.c_str());

    if (method == CODE(kGetAttr)) {

      msgpack::type::tuple<std::string> params;
      req.params().convert(&params);
      GetAttr(req, params.get<0>());

    } else if (method == CODE(kReadLink)) {

      msgpack::type::tuple<std::string, size_t> params;
      req.params().convert(&params);
      ReadLink(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kMkDir)) {

      msgpack::type::tuple<std::string, mode_t> params;
      req.params().convert(&params);
      MkDir(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kUnlink)) {

      msgpack::type::tuple<std::string> params;
      req.params().convert(&params);
      Unlink(req, params.get<0>());

    } else if (method == CODE(kRmDir)) {

      msgpack::type::tuple<std::string> params;
      req.params().convert(&params);
      RmDir(req, params.get<0>());

    } else if (method == CODE(kSymlink)) {

      msgpack::type::tuple<std::string, std::string> params;
      req.params().convert(&params);
      Symlink(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kRename)) {

      msgpack::type::tuple<std::string, std::string> params;
      req.params().convert(&params);
      Rename(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kLink)) {

      msgpack::type::tuple<std::string, std::string> params;
      req.params().convert(&params);
      Link(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kChmod)) {

      msgpack::type::tuple<std::string, mode_t> params;
      req.params().convert(&params);
      Chmod(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kChown)) {

      msgpack::type::tuple<std::string, uid_t, gid_t> params;
      req.params().convert(&params);
      Chown(req, params.get<0>(), params.get<1>(), params.get<2>());

    } else if (method == CODE(kTruncate)) {

      msgpack::type::tuple<std::string, off_t> params;
      req.params().convert(&params);
      Truncate(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kOpen)) {

      msgpack::type::tuple<std::string, int> params;
      req.params().convert(&params);
      Open(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kRead)) {

      msgpack::type::tuple<std::string, int, size_t, off_t> params;
      req.params().convert(&params);
      Read(req, params.get<0>(), params.get<1>(), params.get<2>(), params.get<3>());

    } else if (method == CODE(kWrite)) {

      msgpack::type::tuple<std::string, int, off_t, msgpack::type::raw_ref> params;
      req.params().convert(&params);
      Write(req, params.get<0>(), params.get<1>(), params.get<2>(), params.get<3>());

    } else if (method == CODE(kStatFs)) {

      msgpack::type::tuple<std::string> params;
      req.params().convert(&params);
      StatFs(req, params.get<0>());

    } else if (method == CODE(kFlush)) {

      msgpack::type::tuple<std::string, int> params;
      req.params().convert(&params);
      Flush(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kRelease)) {

      msgpack::type::tuple<std::string, int> params;
      req.params().convert(&params);
      Release(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kFSync)) {
      
      msgpack::type::tuple<std::string, int, int> params;
      req.params().convert(&params);
      FSync(req, params.get<0>(), params.get<1>(), params.get<2>());

    } else if (method == CODE(kReadDir)) {

      msgpack::type::tuple<std::string, off_t, int> params;
      req.params().convert(&params);
      ReadDir(req, params.get<0>(), params.get<1>(), params.get<2>());

    } else if (method == CODE(kFSyncDir)) {

      msgpack::type::tuple<std::string, int> params;
      req.params().convert(&params);
      FSyncDir(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kSetXAttr)) {

      msgpack::type::tuple<std::string, std::string, std::string, size_t, int> params;
      req.params().convert(&params);
      SetXAttr(req, params.get<0>(), params.get<1>(), params.get<2>(), params.get<3>(), params.get<4>());

    } else if (method == CODE(kGetXAttr)) {

      msgpack::type::tuple<std::string, std::string, std::string, size_t> params;
      req.params().convert(&params);
      GetXAttr(req, params.get<0>(), params.get<1>(), params.get<2>(), params.get<3>());

    } else if (method == CODE(kListXAttr)) {

      msgpack::type::tuple<std::string, std::string, size_t> params;
      req.params().convert(&params);
      ListXAttr(req, params.get<0>(), params.get<1>(), params.get<2>());

    } else if (method == CODE(kRemoveXAttr)) {

      msgpack::type::tuple<std::string, std::string> params;
      req.params().convert(&params);
      RemoveXAttr(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kAccess)) {

      msgpack::type::tuple<std::string, int> params;
      req.params().convert(&params);
      Access(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kCreate)) {

      msgpack::type::tuple<std::string, int, mode_t> params;
      req.params().convert(&params);
      Create(req, params.get<0>(), params.get<1>(), params.get<2>());

    } else if (method == CODE(kFTruncate)) {

      msgpack::type::tuple<std::string, int, off_t> params;
      req.params().convert(&params);
      FTruncate(req, params.get<0>(), params.get<1>(), params.get<2>());

    } else if (method == CODE(kFGetAttr)) {

      msgpack::type::tuple<std::string, int> params;
      req.params().convert(&params);
      FGetAttr(req, params.get<0>(), params.get<1>());

    } else if (method == CODE(kLock)) {

      msgpack::type::tuple<std::string, int, int> params;
      req.params().convert(&params);
      Lock(req, params.get<0>(), params.get<1>(), params.get<2>());

    } else if (method == CODE(kUtimens)) {

      msgpack::type::tuple<std::string, TimeSpec, TimeSpec> params;
      req.params().convert(&params);
      Utimens(req, params.get<0>(), params.get<1>(), params.get<2>());
      
    } else if (method == CODE(kFilePrevRead)) {

      msgpack::type::tuple<std::string> params;
      req.params().convert(&params);
      FilePrevRead(req, params.get<0>());

    } else if (method == CODE(kFileFlush)) {

      msgpack::type::tuple<std::string> params;
      req.params().convert(&params);
      FileFlush(req, params.get<0>());

    } else if (method == CODE(kLocalFileExport)) {

      LocalFileExport(req);

    } else {

      req.error(msgpack::rpc::NO_METHOD_ERROR);

    }

  } catch (msgpack::type_error& e) {
    req.error(msgpack::rpc::ARGUMENT_ERROR);
  } catch (std::exception& e) {
    req.error(std::string(e.what()));
  }

}

/**
 * @breaf MsgPack処理振り分け
 * @param path 検索ディレクトリパス
 */
void BurstBuffer::DuplicateDirSecondaryToLocal(std::string path) {
  DIR *dp;
  struct dirent *ent;
  struct stat st;
  std::string local_path = md_manager_.local_path("");
  std::string second_path = md_manager_.secondary_path("");

  dp = opendir(path.c_str());
  if (dp == NULL) {
    return;
  }

  while ((ent = readdir(dp)) != NULL) {
    std::string fname = path + "/" + ent->d_name;

    // directory
    if (boost::filesystem::is_directory(fname)) {
      if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
        std::string tmp = local_path + fname.substr(second_path.length());
        if (!boost::filesystem::exists(tmp)) {
          struct stat st;
          if (lstat(fname.c_str(), &st) == 0) {
            mkdir(tmp.c_str(), st.st_mode);
            DMSG("Duplicate Directory : %s \n", tmp.c_str());
          }
        }
        DuplicateDirSecondaryToLocal(fname);
      }
    }
  }

  closedir(dp);
}


} // namesapce cbb
