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
#ifndef CBB_BURST_BUFFER_H_
#define CBB_BURST_BUFFER_H_

#include <jubatus/msgpack/rpc/server.h>
#include "meta_data_manager.h"
#include "local_file_exporter.h"

namespace cbb {

// CBBモジュール（サーバー側）のメイン処理クラス
class BurstBuffer : public msgpack::rpc::server::base {

 public:

  BurstBuffer(std::string local_storage_root_path, std::string secondary_storage_root_path, int interval_time);
  virtual ~BurstBuffer();

  void GetAttr(msgpack::rpc::request req, const std::string &path);
  void ReadLink(msgpack::rpc::request req, const std::string &path, size_t size);
  void MkDir(msgpack::rpc::request req, const std::string &path, mode_t mode);
  void Unlink(msgpack::rpc::request req, const std::string &path);
  void RmDir(msgpack::rpc::request req, const std::string &path);
  void Symlink(msgpack::rpc::request req, const std::string &path, const std::string &link); //*
  void Rename(msgpack::rpc::request req, const std::string &old_path, const std::string &new_path);
  void Link(msgpack::rpc::request req, const std::string &path, const std::string &newpath); //*
  void Chmod(msgpack::rpc::request req, const std::string &path, mode_t mode);
  void Chown(msgpack::rpc::request req, const std::string &path, uid_t uid, gid_t gid);
  void Truncate(msgpack::rpc::request req, const std::string &path, off_t size);

  void Open(msgpack::rpc::request req, const std::string &path, int flags);
  void Read(msgpack::rpc::request req, const std::string &path, int fd, size_t size, off_t offset);
  void Write(msgpack::rpc::request req, const std::string &path, int fd, off_t offset, const msgpack::type::raw_ref &raw);
  void StatFs(msgpack::rpc::request req, const std::string &path); //*
  void Flush(msgpack::rpc::request req, const std::string &path, int fd);
  void Release(msgpack::rpc::request req, const std::string &path, int fd);
  void FSync(msgpack::rpc::request req, const std::string &path, int fd, int datasync);

  void ReadDir(msgpack::rpc::request req, const std::string &path, off_t offset, int type);
  void FSyncDir(msgpack::rpc::request req, const std::string &path, int datasync); //*

  void SetXAttr(msgpack::rpc::request req, const std::string &path, const std::string &name, const std::string &value, size_t size, int flags); //*
  void GetXAttr(msgpack::rpc::request req, const std::string &path, const std::string &name, const std::string &value, size_t size); //*
  void ListXAttr(msgpack::rpc::request req, const std::string &path, const std::string &list, size_t size); //*
  void RemoveXAttr(msgpack::rpc::request req, const std::string &path, const std::string &name); //*

  void Access(msgpack::rpc::request req, const std::string &path, int mode);
  void Create(msgpack::rpc::request req, const std::string &path, int flags, mode_t mode);
  void FTruncate(msgpack::rpc::request req, const std::string &path, int fd, off_t size);
  void FGetAttr(msgpack::rpc::request req, const std::string &path, int fd); //*
  void Lock(msgpack::rpc::request req, const std::string &path, int fd, int cmd); //*
  void Utimens(msgpack::rpc::request req, const std::string &path, const TimeSpec &time0, const TimeSpec &time1);

  void FilePrevRead(msgpack::rpc::request req, const std::string &path);
  void FileFlush(msgpack::rpc::request req, const std::string &path);
  void LocalFileExport(msgpack::rpc::request req);

  void dispatch(msgpack::rpc::request req);

private:

  int ReadDirInternal(const std::string &path, off_t offset, FileStats &file_stats);
  void DuplicateDirSecondaryToLocal(std::string path);


  MetaDataManager md_manager_;
  LocalFileExporter lf_exporter_;
};

} // namesapce cbb

#endif // CBB_BURST_BUFFER_H_
