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

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "common/error.h"
#include "common/common.h"
#include "util/hash/hash_calc_md5.h"
#include "util/mutex.h"
#include "burst_buffer_client.h"

static cbb::Mutex g_fuse_mutex;


#define USE_SESSION_POOL_FOR_IO

#define RETRY_MAX 3       // 再実行の上限回数
#define RETRY_INTERVAL 1  // 再実行の間隔(秒

#define RPC_RETRY_EXCEPTION_COMMON_HANDLER()  \
  /* 再実行回数の上限に達したら諦める */               \
  if (--retry_count <= 0) throw;              \
                                                     \
  /* 再実行の準備: 一旦 接続を明示的に破棄 */          \
  c.close();                                  \
                                                     \
  /* 間隔をおいて再実行 */                            \
  std::cerr << e.what() << std::endl;         \
  ::sleep(RETRY_INTERVAL);                    \
  continue


#define MSGPACK_CLIENT_CALL(operation)                      \
  try {                                                     \
    int retry_count = 0;                                    \
    g_fuse_mutex.Lock();                                    \
    while (true) {                                          \
      try {                                                 \
        operation ;                                         \
        break;                                              \
      } catch (msgpack::rpc::connection_closed_error &e) {  \
        RPC_RETRY_EXCEPTION_COMMON_HANDLER();               \
      } catch (msgpack::rpc::system_error &e) {             \
        RPC_RETRY_EXCEPTION_COMMON_HANDLER();               \
      } catch (msgpack::rpc::timeout_error &e) {            \
        RPC_RETRY_EXCEPTION_COMMON_HANDLER();               \
      }                                                              \
    }                                                                \
    g_fuse_mutex.Unlock();                                  \
  } catch (msgpack::rpc::remote_error &e) {                 \
    g_fuse_mutex.Unlock();                                  \
    std::cerr << "Error " << e.what() << std::endl;         \
    abort();                                                \
  }


uint32_t addr_to_binary(const char *ipv4_addr) {
  struct in_addr addr;
	int ret = inet_aton(ipv4_addr, &addr);
	if (ret == 0) {
	  return -1;
	}
	return addr.s_addr;
}


// CBFSモジュール（クライアント側）のメイン処理クラス
namespace cbb {

/**
 * @breaf constractor
 */
BurstBufferClient::BurstBufferClient() {
	Thread::Init();
}

/**
 * @breaf destractor
 */
BurstBufferClient::~BurstBufferClient() {
	Thread::Release();
}


/**
 * @breaf ファイル属性取得
 * @param path ファイルパス
 * @param file_stat_ptr ファイルステータス構造体ポインタ
 * @return Error値
 */
Error BurstBufferClient::GetAttr(const char *path, FileStat *file_stat_ptr) {
DMSG("GetAttr [%s] - ", path);
  std::string link_path;
  return GetAttrEx(path, file_stat_ptr, link_path);
}

/**
 * @breaf ファイル属性取得
 * @param path ファイルパス
 * @param file_stat_ptr ファイルステータス構造体ポインタ
 * @return Error値
 */
Error BurstBufferClient::GetAttrExInternal(const char *path, FileStat *file_stat_ptr, std::string &link_path) {
DMSG("GetAttrExInternal [%s] \n", path);

  link_path = "";

  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  typedef msgpack::type::tuple<Error, FileStat, std::string> Result;
  MSGPACK_CLIENT_CALL(
      Result result  = c.call(CODE(kGetAttr), std::string(path)).get<Result>();
      error = result.get<0>();
      *file_stat_ptr = result.get<1>();
      link_path = result.get<2>();
  );

  return error;
}

/**
 * @breaf ファイル属性取得
 * @param path ファイルパス
 * @param file_stat_ptr ファイルステータス構造体ポインタ
 * @return Error値
 */
Error BurstBufferClient::GetAttrEx(const char *path, FileStat *file_stat_ptr, std::string &link_path) {
DMSG("GetAttrEx [%s] \n", path);

  // virtual symlink check
  std::string target = create_virtual_symlink(path);
  Error error = GetAttrExInternal(target.c_str(), file_stat_ptr, link_path);

  if (error < 0) {
    // virtual link check
    target = create_virtual_link(path);
    error = GetAttrExInternal(target.c_str(), file_stat_ptr, link_path);
    if (error < 0) {
      error = GetAttrExInternal(path, file_stat_ptr, link_path);
    } else if (!link_path.empty()) {
      target = link_path;
      GetAttrExInternal(target.c_str(), file_stat_ptr, link_path);
//      file_stat_ptr->st_mode |= S_IFLNK; // symlink属性付加
    }
  } else if (!link_path.empty()) {
    file_stat_ptr->st_mode |= S_IFLNK; // symlink属性付加
  }

  return error;
}

/**
 * @breaf リンク情報取得
 * @param path ファイルパス
 * @param buf バッファポインタ
 * @param size バッファサイズ
 * @return Error値
 */
Error BurstBufferClient::ReadLink(const char *path, char *buf, size_t size) {
  *buf = NULL;

  std::string link_path;
  FileStat file_stat;

  // virtual symlink check
  std::string target = create_virtual_symlink(path);
  Error error = GetAttrExInternal(target.c_str(), &file_stat, link_path);

  if (error < 0) {
    // virtual link check
    target = create_virtual_link(path);
    error = GetAttrExInternal(target.c_str(), &file_stat, link_path);

    if (error < 0) {
      link_path = "";

      std::string bb_host;
      uint16_t bb_port;

      error = GetBurstBuffer(path, &bb_host, &bb_port);
      if (error != kCBBSuccess)
        return error;

#ifdef USE_SESSION_POOL_FOR_IO
      msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
      msgpack::rpc::client c(bb_host, bb_port);
#endif

      msgpack::rpc::auto_zone zone;

      typedef msgpack::type::tuple<Error, msgpack::type::raw_ref> Result;
      MSGPACK_CLIENT_CALL(
          Result result = c.call(CODE(kReadLink), std::string(path), size).get<Result>(&zone);
          error = result.get<0>();
          msgpack::type::raw_ref data = result.get<1>();
          std::memcpy(buf, data.ptr, data.size);
      );
    } else if (!link_path.empty()) {
      strcpy(buf, link_path.c_str());
    }
  } else if (!link_path.empty()) {
    strcpy(buf, link_path.c_str());
  }

  return (error >= 0 ? 0: error);
}

/**
 * @breaf ディレクトリ作成
 * @param path ファイルパス
 * @param mode モード値
 * @return Error値
 */
Error BurstBufferClient::MkDir(const char *path, mode_t mode) {
  Error error;

  BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {

#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
    msgpack::rpc::client c(info.host, info.port);
#endif

    MSGPACK_CLIENT_CALL(
        error = c.call(CODE(kMkDir), std::string(path), mode).get<Error>();
    );
  }
  
  return error;
}

/**
 * @breaf ファイル削除
 * @param path ファイルパス
 * @return Error値
 */
Error BurstBufferClient::UnlinkInternal(const char *path, bool is_all_server) {
DMSG("UnlinkInternal [%s] \n", path);
  Error error;

  if (is_all_server) {
    BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {

#ifdef USE_SESSION_POOL_FOR_IO
      msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
      msgpack::rpc::client c(info.host, info.port);
#endif

      MSGPACK_CLIENT_CALL(
          error = c.call(CODE(kUnlink), std::string(path)).get<Error>();
      );
    }
  } else {

    std::string bb_host;
    uint16_t bb_port;

    error = GetBurstBuffer(path, &bb_host, &bb_port);
    if (error != kCBBSuccess)
      return error;

#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
    msgpack::rpc::client c(bb_host, bb_port);
#endif

    MSGPACK_CLIENT_CALL(
        error = c.call(CODE(kUnlink), std::string(path)).get<Error>();
    );
  }
  
  return error;
}

/**
 * @breaf ファイル削除
 * @param path ファイルパス
 * @return Error値
 */
Error BurstBufferClient::Unlink(const char *path) {
DMSG("Unlink [%s] \n", path);
  FileStat file_stat;
  std::string link_path;

  // virtual symlink check
  std::string target = create_virtual_symlink(path);
  Error error = GetAttrExInternal(target.c_str(), &file_stat, link_path);

  if (error < 0) {
    // virtual link check
    target = create_virtual_link(path);
    error = GetAttrExInternal(target.c_str(), &file_stat, link_path);
    if (error < 0) {
      error = UnlinkInternal(path, true);
    } else {
      error = UnlinkInternal(target.c_str(), false);
    }
  } else {
    error = UnlinkInternal(target.c_str(), false);
  }

  return error;
}

/**
 * @breaf ディレクトリ削除
 * @param path ファイルパス
 * @return Error値
 */
Error BurstBufferClient::RmDir(const char *path) {
  Error error;

  BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {

#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
    msgpack::rpc::client c(info.host, info.port);
#endif

    MSGPACK_CLIENT_CALL(
        error = c.call(CODE(kRmDir), std::string(path)).get<Error>();
    );
  }
  
  return error;
}

/**
 * @breaf シンボリックリンク
 * @param path ファイルパス
 * @param link リンク
 * @return Error値
 */
Error BurstBufferClient::Symlink(const char *path, const char *link) {
  Error error;
  FileStat file_stat;
  std::string link_path;

  error = GetAttrEx(path, &file_stat, link_path);

  // ディレクトリの場合
  if (error >= 0 && S_ISDIR(file_stat.st_mode)) {
//DMSG("Symlink - Dir \n");

    BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {

#ifdef USE_SESSION_POOL_FOR_IO
      msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
      msgpack::rpc::client c(info.host, info.port);
#endif

      MSGPACK_CLIENT_CALL(
          error = c.call(CODE(kSymlink), std::string(path), std::string(link)).get<Error>();
      );
    }

  } else {
//DMSG("Symlink - Virtual Symlink \n");

    std::string bb_host;
    uint16_t bb_port;

    link_path = create_virtual_symlink(link);
    Error error = GetBurstBuffer(link_path.c_str(), &bb_host, &bb_port);
    if (error != kCBBSuccess)
      return error;

#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
    msgpack::rpc::client c(bb_host, bb_port);
#endif

    MSGPACK_CLIENT_CALL(
        error = c.call(CODE(kSymlink), std::string(path), link_path).get<Error>();
    );
  }

  return error;
}

/**
 * @breaf リネーム
 * @param old_path 変更前ファイルパス
 * @param new_path 変更後ファイルパス
 * @return Error値
 */
Error BurstBufferClient::Rename(const char *old_path, const char *new_path) {
  Error error;

  // ファイル、ディレクトリチェック
  FileStat file_stat;
  std::string link_path;

  error = GetAttrEx(old_path, &file_stat, link_path);

  // ディレクトリの場合
  if (error >= 0 && S_ISDIR(file_stat.st_mode)) {
    // すべてのサーバーにリネーム命令を実行する
    BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {

#ifdef USE_SESSION_POOL_FOR_IO
      msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
      msgpack::rpc::client c(info.host, info.port);
#endif

      MSGPACK_CLIENT_CALL(
          error = c.call(CODE(kRename), std::string(old_path), std::string(new_path)).get<Error>();
      );
    }

  } else {
    std::string bb_host;
    uint16_t bb_port;

    Error error = GetBurstBuffer(old_path, &bb_host, &bb_port);
    if (error != kCBBSuccess)
      return error;

#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
    msgpack::rpc::client c(bb_host, bb_port);
#endif

    MSGPACK_CLIENT_CALL(
        error = c.call(CODE(kRename), std::string(old_path), std::string(new_path)).get<Error>();
    );
  }

  return error;
}

/**
 * @breaf リンク
 * @param path ファイルパス
 * @param newpath 新ファイルパス
 * @return Error値
 */
Error BurstBufferClient::Link(const char *path, const char *newpath) {
  Error error;
  FileStat file_stat;
  std::string link_path;

  error = GetAttrEx(path, &file_stat, link_path);

  // ディレクトリの場合
  if (S_ISDIR(file_stat.st_mode)) {
//DMSG("Link - Dir \n");

    BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {

#ifdef USE_SESSION_POOL_FOR_IO
      msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
      msgpack::rpc::client c(info.host, info.port);
#endif

      MSGPACK_CLIENT_CALL(
          error = c.call(CODE(kLink), std::string(path), std::string(newpath)).get<Error>();
      );
    }

  } else {
//DMSG("Link - Virtual Link \n");

    std::string bb_host;
    uint16_t bb_port;
    std::string link_path = create_virtual_link(newpath);

    Error error = GetBurstBuffer(link_path.c_str(), &bb_host, &bb_port);
    if (error != kCBBSuccess)
      return error;

#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
    msgpack::rpc::client c(bb_host, bb_port);
#endif

    MSGPACK_CLIENT_CALL(
        error = c.call(CODE(kLink), std::string(path), link_path).get<Error>();
    );
  }

  return error;
}

/**
 * @breaf ファイル属性変更
 * @param path ファイルパス
 * @param mode モード値
 * @return Error値
 */
Error BurstBufferClient::Chmod(const char *path, mode_t mode) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kChmod), std::string(path), mode).get<Error>();
  );

  return error;
}

/**
 * @breaf ファイルオーナー変更
 * @param path ファイルパス
 * @param uid UID
 * @param gid GID
 * @return Error値
 */
Error BurstBufferClient::Chown(const char *path, uid_t uid, gid_t gid) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kChown), std::string(path), uid, gid).get<Error>();
  );
  
  return error;
}

/**
 * @breaf ファイルサイズ変更
 * @param path ファイルパス
 * @param size サイズ
 * @return Error値
 */
Error BurstBufferClient::Truncate(const char *path, off_t size) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kTruncate), std::string(path), size).get<Error>();
  );
 
  return error;
}



/**
 * @breaf ファイルオープン
 * @param path ファイルパス
 * @param flags フラグ
 * @param file_ptr ファイル情報ポインタ
 * @return Error値
 */
Error BurstBufferClient::Open(const char* path, int flags, File *file_ptr) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;
 
  file_ptr->path = path;
  file_ptr->bb_host = bb_host;
  file_ptr->bb_port = bb_port;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  int fd = -1;
  MSGPACK_CLIENT_CALL(
      fd = c.call(CODE(kOpen), std::string(path), flags).get<int>();
  );

  if (fd < 0)
    return static_cast<Error>(fd);

  file_ptr->fd_org = fd;
  file_ptr->fd = ((uint64_t)addr_to_binary(bb_host.c_str()) << 32) | fd;

  // 先読み開始
  StartPrevFileRead(path);

  return kCBBSuccess;
}

/**
 * @breaf ファイル読み込み
 * @param path ファイルパス
 * @param buf バッファポインタ
 * @param size サイズ
 * @param offset オフセット
 * @param ssize_ptr サイズ保存ポインタ
 * @return Error値
 */
Error BurstBufferClient::Read(const File &file, char *buf, size_t size, off_t offset, ssize_t *ssize_ptr) {

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(file.bb_host, file.bb_port);
#else
  msgpack::rpc::client c(file.bb_host, file.bb_port);
#endif

  ssize_t ssize = 0;
  msgpack::type::raw_ref raw;
  msgpack::rpc::auto_zone zone;
  
  typedef msgpack::type::tuple<ssize_t, msgpack::type::raw_ref> Result;
  MSGPACK_CLIENT_CALL(
      Result result =  c.call(CODE(kRead), file.path, file.fd_org, size, offset).get<Result>(&zone);
      ssize = result.get<0>();
      raw = result.get<1>();
  );

  if (ssize < 0)
    return static_cast<Error>(ssize);

  *ssize_ptr = ssize;
  std::memcpy(buf, raw.ptr, raw.size);
  
  return kCBBSuccess;
}

/**
 * @breaf ファイル書き込み
 * @param file ファイル情報
 * @param buf バッファポインタ
 * @param size サイズ
 * @param offset オフセット
 * @param ssize_ptr サイズ保存ポインタ
 * @return Error値
 */
Error BurstBufferClient::Write(const File &file, const char *buf, size_t size, off_t offset, ssize_t *ssize_ptr) {

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(file.bb_host, file.bb_port);
#else
  msgpack::rpc::client c(file.bb_host, file.bb_port);
#endif

  msgpack::type::raw_ref raw(buf, size);

  ssize_t ssize = 0;
  MSGPACK_CLIENT_CALL(
      ssize = c.call(CODE(kWrite), file.path, file.fd_org, offset, raw).get<ssize_t>();
  );

  if (ssize < 0)
    return static_cast<Error>(ssize);

  *ssize_ptr = ssize;
  
  return kCBBSuccess;
}

/**
 * @breaf StatFs
 * @param path ファイルパス
 * @param buf statvfs情報
 * @return Error値
 */
Error BurstBufferClient::StatFs(const char *path, struct statvfs *buf) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  typedef msgpack::type::tuple<Error, StatVfs> Result;
  MSGPACK_CLIENT_CALL(
      Result result  = c.call(CODE(kStatFs), std::string(path)).get<Result>();
      error = result.get<0>();
      *buf = result.get<1>();
  );

  return error;
}

/**
 * @breaf ファイルフラッシュ
 * @param file ファイル情報
 * @return Error値
 */
Error BurstBufferClient::Flush(const File &file) {

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(file.bb_host, file.bb_port);
#else
  msgpack::rpc::client c(file.bb_host, file.bb_port);
#endif

  Error error = kCBBSuccess;
  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kFlush), file.path, static_cast<int>(file.fd_org)).get<Error>();
  );

  return error;
}

/**
 * @breaf ファイル開放（クローズ）
 * @param file ファイル情報
 * @return Error値
 */
Error BurstBufferClient::Release(const File &file) {

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(file.bb_host, file.bb_port);
#else
  msgpack::rpc::client c(file.bb_host, file.bb_port);
#endif

  Error error = kCBBSuccess;
  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kRelease), file.path, static_cast<int>(file.fd_org)).get<Error>();
  );

  return error;
}

/**
 * @breaf ファイル同期
 * @param file ファイル情報
 * @param datasync データ同期
 * @return Error値
 */
Error BurstBufferClient::FSync(const File &file, int datasync) {

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(file.bb_host, file.bb_port);
#else
  msgpack::rpc::client c(file.bb_host, file.bb_port);
#endif

  Error error = kCBBSuccess;
  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kFSync), file.path, static_cast<int>(file.fd_org), datasync).get<Error>();
  );

  return error;
}



/**
 * @breaf ディレクトリリスト読み込み
 * @param path ファイルパス
 * @param offset オフセット
 * @param file_stats_ptr ファイル情報配列ポインタ
 * @return Error値
 */
Error BurstBufferClient::ReadDir(const char *path, off_t offset, FileStats *file_stats_ptr, int type) {
  Error error;

  file_stats_ptr->clear();

  BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {
#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
    msgpack::rpc::client c(info.host, info.port);
#endif

    FileStats file_stats;

    typedef msgpack::type::tuple<Error, FileStats> Result;
    MSGPACK_CLIENT_CALL(
      Result result = c.call(CODE(kReadDir), std::string(path), offset, type).get<Result>();
      error = result.get<0>();
      file_stats = result.get<1>();
    );

    file_stats_ptr->insert(file_stats.begin(), file_stats.end());
  }

  return error;
}

/**
 * @breaf ディレクトリ同期
 * @param path ディレクトリパス
 * @param datasync データ同期
 * @param file ファイル情報
 * @return Error値
 */
Error BurstBufferClient::FSyncDir(const char *path, int datasync, const File &file) {
  Error error;

  BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {
#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
    msgpack::rpc::client c(info.host, info.port);
#endif

    MSGPACK_CLIENT_CALL(
        error = c.call(CODE(kFSync), std::string(path), datasync).get<Error>();
    );
  }

  return error;
}



/**
 * @breaf 拡張ファイル属性設定
 * @param path ファイルパス
 * @param name 名前
 * @param value 設定値
 * @param size サイズ
 * @param flags フラグ
 * @return Error値
 */
Error BurstBufferClient::SetXAttr(const char *path, const char *name, const char *value, size_t size, int flags) {
  Error error;

  // ファイル、ディレクトリチェック
  FileStat file_stat;
  std::string link_path;

  error = GetAttrEx(path, &file_stat, link_path);

  // ディレクトリの場合
  if (error >= 0 && S_ISDIR(file_stat.st_mode)) {
    // すべてのサーバーにリネーム命令を実行する
    BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {

#ifdef USE_SESSION_POOL_FOR_IO
      msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
      msgpack::rpc::client c(info.host, info.port);
#endif

      MSGPACK_CLIENT_CALL(
          error = c.call(CODE(kSetXAttr), std::string(path), std::string(name), std::string(value), size, flags).get<Error>();
      );
    }

  } else {
    std::string bb_host;
    uint16_t bb_port;

    Error error = GetBurstBuffer(path, &bb_host, &bb_port);
    if (error != kCBBSuccess)
      return error;

#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
    msgpack::rpc::client c(bb_host, bb_port);
#endif

    MSGPACK_CLIENT_CALL(
        error = c.call(CODE(kSetXAttr), std::string(path), std::string(name), std::string(value), size, flags).get<Error>();
    );
  }

  return error;
}

/**
 * @breaf 拡張ファイル属性取得
 * @param path ファイルパス
 * @param name 名前
 * @param value 設定値
 * @param size サイズ
 * @return Error値
 */
Error BurstBufferClient::GetXAttr(const char *path, const char *name, char *value, size_t size) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  typedef msgpack::type::tuple<size_t, std::string> Result;
  MSGPACK_CLIENT_CALL(
      Result result = c.call(CODE(kGetXAttr), std::string(path), std::string(name), std::string(value != NULL ? value : ""), size).get<Result>();
      error = result.get<0>();
      std::string value_result = result.get<1>();
      if (value != NULL) {
        strcpy(value, value_result.c_str());
      }
  );

  //return error;
  return kCBBSuccess;
}

/**
 * @breaf リスト属性
 * @param path ファイルパス
 * @param list リスト
 * @param size サイズ
 * @return Error値
 */
Error BurstBufferClient::ListXAttr(const char *path, char *list, size_t size) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  typedef msgpack::type::tuple<size_t, std::string> Result;
  MSGPACK_CLIENT_CALL(
      Result result = c.call(CODE(kListXAttr), std::string(path), std::string(list != NULL ? list: ""), size).get<Result>();
      error = result.get<0>();
      std::string list_result = result.get<1>();
      if (list != NULL) {
        strcpy(list, list_result.c_str());
      }
  );

  //return error;
  return kCBBSuccess;
}

/**
 * @breaf 拡張ファイル属性削除
 * @param path ファイルパス
 * @param name 名前
 * @return Error値
 */
Error BurstBufferClient::RemoveXAttr(const char *path, const char *name) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kRemoveXAttr), std::string(path), std::string(name)).get<Error>();
  );

  return error;
}



/**
 * @breaf 初期化
 * @param config_path 設定ファイルパス
 * @return Error値
 */
Error BurstBufferClient::Init(const char *config_path) {
  if (!settings_.Load(config_path, false)) {
    return kCBBUnknownError;
  }

  select_server_.Init(settings_, new cbb::HashCalcMD5());
  g_fuse_mutex.Init();

  BOOST_FOREACH(std::string host, settings_.client_hosts()) {
    DMSG("host = %s\n", host.c_str());
  }
  DMSG("port = %d\n", settings_.client_port());
  DMSG("------------------------\n");

  life_.reset(new msgpack::zone());
  return static_cast<Error>(0);
}

/**
 * @breaf 破棄
 * @return Error値
 */
Error BurstBufferClient::Destroy() {
  Thread::Release();
  return kCBBSuccess;
}



/**
 * @breaf ファイルアクセス
 * @param path ファイルパス
 * @param mode モード値
 * @return Error値
 */
Error BurstBufferClient::Access(const char *path, int mode) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kAccess), std::string(path), mode).get<Error>();
  );

  return error;
}

/**
 * @breaf ファイルクリエイト
 * @param path ファイルパス
 * @param flags フラグ
 * @param mode モード値
 * @param file_ptr ファイル情報ポインタ
 * @return Error値
 */
Error BurstBufferClient::Create(const char *path, int flags,  mode_t mode, File *file_ptr) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

  file_ptr->path = path;
  file_ptr->bb_host = bb_host;
  file_ptr->bb_port = bb_port;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  int fd = -1;
  MSGPACK_CLIENT_CALL(
      fd = c.call(CODE(kCreate), std::string(path), flags, mode).get<int>();
  );

  if (fd < 0)
    return static_cast<Error>(fd);

  file_ptr->fd_org = fd;
  file_ptr->fd = ((uint64_t)addr_to_binary(bb_host.c_str()) << 32) | fd;

  return kCBBSuccess;
}

/**
 * @breaf ファイルサイズ変更
 * @param file ファイル情報
 * @param size サイズ
 * @return Error値
 */
Error BurstBufferClient::FTruncate(const File &file, off_t size) {

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(file.bb_host, file.bb_port);
#else
  msgpack::rpc::client c(file.bb_host, file.bb_port);
#endif

  Error error = kCBBSuccess;

  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kFTruncate), std::string(file.path), file.fd_org, size).get<Error>();
  );

  return error;
}



/**
 * @breaf ファイル属性取得 (ファイルディスクリプタ版)
 * @param path ファイルパス
 * @param file_stat_ptr ファイルステータスポインタ
 * @param file ファイル情報
 * @return Error値
 */
Error BurstBufferClient::FGetAttr(const char *path, FileStat *file_stat_ptr, const File &file) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  typedef msgpack::type::tuple<Error, FileStat> Result;
  MSGPACK_CLIENT_CALL(
      Result result  = c.call(CODE(kFGetAttr), std::string(path), file.fd_org).get<Result>();
      error = result.get<0>();
      *file_stat_ptr = result.get<1>();
  );

  return error;
}

/**
 * @breaf ファイルロック
 * @param path ファイルパス
 * @param file ファイル情報
 * @param cmd コマンド
 * @param lockbuf ファイルロック情報
 * @return Error値
 */
Error BurstBufferClient::Lock(const char *path, const File &file, int cmd, struct flock *lockbuf) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  typedef msgpack::type::tuple<Error, FLock> Result;
  MSGPACK_CLIENT_CALL(
      Result result  = c.call(CODE(kLock), std::string(path), file.fd_org).get<Result>();
      error = result.get<0>();
      *lockbuf = result.get<1>();
  );

  return error;
}

/**
 * @breaf ファイル日時変更
 * @param path ファイルパス
 * @param times 日時情報
 * @return Error値
 */
Error BurstBufferClient::Utimens(const char *path, const TimeSpec times[2]) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kUtimens), std::string(path), times[0], times[1]).get<Error>();
  );

  return error;
}



/**
 * @breaf ファイル先読み
 * @param path ファイルパス
 * @return Error値
 */
Error BurstBufferClient::FilePrevRead(const char *path) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kFilePrevRead), std::string(path)).get<Error>();
  );

  return error;
}

/**
 * @breaf ファイルフラッシュ
 * @param path ファイルパス
 * @return Error値
 */
Error BurstBufferClient::FileFlush(const char *path) {
  std::string bb_host;
  uint16_t bb_port;

  Error error = GetBurstBuffer(path, &bb_host, &bb_port);
  if (error != kCBBSuccess)
    return error;

#ifdef USE_SESSION_POOL_FOR_IO
  msgpack::rpc::session c = session_pool_.get_session(bb_host, bb_port);
#else
  msgpack::rpc::client c(bb_host, bb_port);
#endif

  MSGPACK_CLIENT_CALL(
      error = c.call(CODE(kFileFlush), std::string(path)).get<Error>();
  );

  return error;
}


/**
 * @breaf ファイルフラッシュ
 * @param path ファイルパス
 * @return Error値
 */
Error BurstBufferClient::LocalFileExport() {
  Error error;

  BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {
#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
    msgpack::rpc::client c(info.host, info.port);
#endif

    MSGPACK_CLIENT_CALL(
        error = c.call(CODE(kLocalFileExport)).get<Error>();
    );
  }

  return error;
}


/**
 * @see Thread::ThreadCall
 * @breaf 先読みファイルのコピー
 * @return 呼び出しを継続するかどうか
 */
bool BurstBufferClient::ThreadCall(void *user_data) {
  FileStats dir_files;
  Error error;
  std::string host;
  uint16_t port;

  dir_files.clear();

  BOOST_FOREACH(ServerInfo info, select_server_.server_list()) {
#ifdef USE_SESSION_POOL_FOR_IO
    msgpack::rpc::session c = session_pool_.get_session(info.host, info.port);
#else
    msgpack::rpc::client c(info.host, info.port);
#endif

    FileStats file_stats;

    typedef msgpack::type::tuple<Error, FileStats> Result;
    MSGPACK_CLIENT_CALL(
        Result result = c.call(CODE(kReadDir), std::string(prev_read_dir_.c_str()), (off_t)0, (int)kDirSecondary).get<Result>();
        error = result.get<0>();
        file_stats = result.get<1>();
    );

    dir_files.insert(file_stats.begin(), file_stats.end());
  }

  for (FileStats::iterator it = dir_files.begin(); it != dir_files.end(); it++) {
    std::string filename = it->first;
    std::string path = prev_read_dir_ + filename;

//DMSG("FilePrevRead path = %s \n", path.c_str());

    if (filename == "." || filename == ".." || filename == target_filename_)
      continue;

    error = FilePrevRead(path.c_str());
  }

  return false;
}

/**
 * @breaf BurstBufferサーバー取得
 * @param path ファイルパス
 * @param bb_host_ptr ホスト情報
 * @param bb_port_ptr 接続ポート番号
 * @return Error値
 */
Error BurstBufferClient::GetBurstBuffer(const char *path, std::string *bb_host_ptr, uint16_t *bb_port_ptr) {

  std::string host = "";
  int port = 0;

  select_server_.GetInfo(path, host, port);

  // Should introduce schedule feature here.
  *bb_host_ptr = host;
  *bb_port_ptr = port;

  return kCBBSuccess;
}

/**
 * @breaf ファイル先読み開始
 * @param path ファイルパス
 */
void BurstBufferClient::StartPrevFileRead(const char* path) {
  boost::filesystem::path fpath(path);
  prev_read_dir_ = fpath.parent_path().c_str();
  target_filename_ = fpath.filename().c_str();
  Thread::Create((void *)NULL);
}


} // namespace cbb
