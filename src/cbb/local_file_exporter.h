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
#ifndef CBB_LOCAL_FILE_EXPORTER_H_
#define CBB_LOCAL_FILE_EXPORTER_H_

#include <stdio.h>
#include <time.h>

#include <string>
#include <map>

#include "util/thread.h"
#include "util/mutex.h"

namespace cbb {

class MetaDataManager;

// ローカルストレージのファイルをセカンダリストレージにコピーするクラス
class LocalFileExporter : public Thread {

 public:

  LocalFileExporter() : md_manager_ptr_(NULL) {}
  virtual ~LocalFileExporter() {}

  void Create(MetaDataManager *md_manager_ptr, int interval_time);
  void Release();

  bool Register(const std::string &path);
  void Unregister(const std::string &path);
  void UnregisterAll();

  void ReSearchLocalFiles();
  void CheckLocalFiles();

 protected:
  bool ThreadCall(void *user_data);

 private:

  void SearchLocalFiles(std::string path);

  typedef std::map<std::string, time_t> LocalFiles;
  LocalFiles local_files_;

  Mutex mutex_;
  MetaDataManager *md_manager_ptr_;
};

} // namespace cbb

#endif // CBB_LOCAL_FILE_EXPORTER_H_
