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
#include "local_file_exporter.h"

#include <dirent.h>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <list>

#include "common/error.h"
#include "common/common.h"
#include "meta_data_manager.h"

// ローカルストレージのファイルをセカンダリストレージにコピーするクラス
namespace cbb {


/**
 * @breaf 作成
 * @param md_manager_ptr メタデータマネージャーポインタ
 * @param interval_time 監視時間間隔(msec)
 */
void LocalFileExporter::Create(MetaDataManager *md_manager_ptr, int interval_time) {
  assert(md_manager_ptr != NULL);

//  DMSG("LocalFileExporter::Create : interval time = %d\n", interval_time);

  local_files_.clear();
  mutex_.Init();
  md_manager_ptr_ = md_manager_ptr;

  SearchLocalFiles(md_manager_ptr->local_path(""));

  Thread::Create(NULL, interval_time);
}

/**
 * @breaf 開放
 */
void LocalFileExporter::Release() {
  Thread::Release();
}

/**
 * @breaf テーブル登録
 * @param path ファイルパス
 * @param fd ファイルディスクリプタ
 * @param mutex_file Mutexファイル
 * @return bool 登録結果
 */
bool LocalFileExporter::Register(const std::string &path) {
  std::string filename = md_manager_ptr_->local_path(path);
  boost::system::error_code ec;

  if (!boost::filesystem::exists(filename, ec)) {
    return false;
  }

  std::time_t time = boost::filesystem::last_write_time(filename, ec);
  if (ec) {
    return false;
  }

  mutex_.Lock();

  local_files_[path] = time;
  DMSG("LocalFileExporter::Register : %s : %d\n", path.c_str(), time);

  mutex_.Unlock();

  return true;
}

/**
 * @breaf テーブル登録解除
 * @param path ファイルパス
 */
void LocalFileExporter::Unregister(const std::string &path) {
  mutex_.Lock();

  local_files_.erase(path);
  DMSG("LocalFileExporter::Unregister : %s : %d\n", path.c_str());

  mutex_.Unlock();
}

/**
 * @breaf すべての登録を解除する
 */
void LocalFileExporter::UnregisterAll() {
  mutex_.Lock();

  local_files_.clear();

  mutex_.Unlock();
}

/**
 * @breaf ローカルファイルを再検索して登録する
 */
void LocalFileExporter::ReSearchLocalFiles() {
  UnregisterAll();
  SearchLocalFiles(md_manager_ptr_->local_path(""));
}

/**
 * @breaf 登録されているローカルファイルをチェックする
 */
void LocalFileExporter::CheckLocalFiles() {
  std::list<std::string> erase_filename;
  std::time_t now_time = time(NULL);

  DMSG("LocalFileExporter::CheckLocalFiles\n");

  mutex_.Lock();

  for (LocalFiles::iterator it = local_files_.begin(); it != local_files_.end(); it++) {
    std::string filename = it->first;
    std::time_t mod_time = it->second;
    bool is_copy = true;

    DMSG("check : %s\n", filename.c_str());

    // file copy local to secondary
    std::string source = md_manager_ptr_->local_path(filename);
    std::string destination = md_manager_ptr_->secondary_path(filename);

    if (boost::filesystem::exists(source)) {
      if (boost::filesystem::exists(destination)) {
        const std::time_t last_update_src = boost::filesystem::last_write_time(source);
        const std::time_t last_update_dst = boost::filesystem::last_write_time(destination);

        if (last_update_src <= last_update_dst) {
          is_copy = false;
        }
      }

      if (is_copy) {
        DMSG("copy %s to %s\n", source.c_str(), destination.c_str());
        boost::filesystem::copy_file(source, destination, boost::filesystem::copy_option::overwrite_if_exists);
      }
    } else {
      erase_filename.push_back(filename);
    }
  }

  BOOST_FOREACH(std::string filename, erase_filename) {
    local_files_.erase(filename);
  }

  mutex_.Unlock();
}


/**
 * @see Thread::ThreadCall
 * @breaf LocalStrageからSecondaryStrageへのコピーチェック
 * @return bool 呼び出しを継続するかどうか
 */
bool LocalFileExporter::ThreadCall(void *user_data) {
  CheckLocalFiles();
  return true;
}


/**
 * @breaf ローカルファイルをサーチして登録する
 * @param path 検索ディレクトリパス
 */
void LocalFileExporter::SearchLocalFiles(std::string path) {
  DIR *dp;
  struct dirent *ent;
  struct stat st;
  std::string local_path = md_manager_ptr_->local_path("");

  dp = opendir(path.c_str());
  if (dp == NULL) {
    return;
  }

  while ((ent = readdir(dp)) != NULL) {
    std::string fname = path + "/" + ent->d_name;

    // file
    if (boost::filesystem::is_regular_file(fname)) {
      std::string tmp = fname.substr(local_path.length());
      DMSG("LocalFileExporter::SearchLocalFiles : %s / %s\n", fname.c_str(), tmp.c_str());
      Register(tmp);
    }
    // directory
    else if (boost::filesystem::is_directory(fname)) {
      if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
        SearchLocalFiles(fname);
      }
    }
  }

  closedir(dp);
}


} // namespace 
