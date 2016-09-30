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
#ifndef CBB_META_DATA_MANAGER_H_
#define CBB_META_DATA_MANAGER_H_

#include <stdio.h>
#include <sys/stat.h>
#include <map>
#include <set>
#include <boost/filesystem.hpp>


namespace cbb {

// 各ファイル等のメタデータマネージャークラス
class MetaDataManager {

 public:

  MetaDataManager(std::string local_storage_root_path,
                  std::string secondary_storage_root_path):
      local_storage_root_path_(local_storage_root_path),
      secondary_storage_root_path_(secondary_storage_root_path) {}

  Error Register(const std::string &path, int fd);
  Error Unregister(const std::string &path);
  Error Unregister(const std::string &path, int fd);

  Error GetFileStat(const std::string &path, FileStat *stat_ptr, std::string &link_path);
  Error GetFileStatFD(int fd, FileStat *file_stat_ptr);

  Error Rename(const std::string &old_path, const std::string &new_path);
  Error Chmod(const std::string &path, mode_t mode);
  Error Chown(const std::string &path, uid_t uid, gid_t gid);
  Error Truncate(const std::string &path, off_t size);
  Error FTruncate(const std::string &path, int fd, off_t size);
  Error Access(const std::string &path, mode_t mode);
  Error Utimens(const std::string &path, const TimeSpec &time0, const TimeSpec &time1);

  Error Create(const std::string &path, int flags, mode_t mode);
  Error Open(const std::string &path, int flags);
  Error Read(const std::string &path, int fd, void *buf, size_t size, off_t offset);
  Error Write(const std::string &path, int fd, const void *buf, size_t size, off_t offset);
  Error FSync(const std::string &path, int fd, int is_data);
  Error Flush(const std::string &path, int fd);
  Error Close(const std::string &path, int fd);

  Error CopySecondaryToLocal(const std::string &path);
  Error FileFlush(const std::string &path);

  bool is_buffered(const std::string &path) {
    return (table_.find(path) != table_.end());
  }

  bool exists_on_local(const std::string &path) {
    boost::system::error_code ec;
    std::string target_path = local_path(path);
    return boost::filesystem::exists(target_path, ec);
  }

  bool exists_on_secondary(const std::string &path) {
    boost::system::error_code ec;
    std::string target_path = secondary_path(path);
    return boost::filesystem::exists(target_path, ec);
  }

  const std::string local_path(const std::string &path) {
    std::string slash = path.substr(0, 1) == "/" ? "": "/";
    return  local_storage_root_path_ + slash + path;
  }

  const std::string secondary_path(const std::string &path) {
    std::string slash = path.substr(0, 1) == "/" ? "": "/";
    return secondary_storage_root_path_ + slash + path;
  }

  const std::string target_path(const std::string &path) {
    return exists_on_local(path) ? local_path(path): secondary_path(path);
  }

  std::string remove_storage_path(std::string path) {
    if (path.find(local_path("")) != std::string::npos) {
      path = path.substr(local_path("/").size());
    } else if (path.find(secondary_path("")) != std::string::npos) {
      path = path.substr(secondary_path("/").size());
    }
    return path;
  }

#if 0
  Error GetChildren(const std::string &path);
#endif

 private:

  typedef std::map<std::string, std::set<int> > BufferedFiles;
  BufferedFiles table_;

  const std::string local_storage_root_path_;
  const std::string secondary_storage_root_path_;
};

} // namespace cbb

#endif // CBB_META_DATA_MANAGER_H_
