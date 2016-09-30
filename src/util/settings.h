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
#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <vector>
#include <string>

namespace cbb {

// 設定ファイル読み込みクラス
class Settings {

 public:
  Settings() : server_port_(0), server_thread_(0), client_port_(0), server_interval_time_(0) {}
  Settings(const char *filename, bool is_server) { Load(filename, is_server); }
  virtual ~Settings() {}

  bool Load(const char *filename, bool is_server);

  std::string server_host() { return server_host_; }
  int server_port() { return server_port_; }
  int server_thread() { return server_thread_; }
  std::string server_local_strage_path() { return server_local_strage_path_; }
  std::string server_secondary_storage_path() { return server_secondary_storage_path_; }
  int server_interval_time() { return server_interval_time_; }

  std::vector<std::string> client_hosts() { return client_hosts_; }
  int client_port() { return client_port_; }

 private:
  std::string server_host_;
  int server_port_;
  int server_thread_;
  std::string server_local_strage_path_;
  std::string server_secondary_storage_path_;
  int server_interval_time_;

  std::vector<std::string> client_hosts_;
  int client_port_;
};

} // namesapce cbb

#endif // SETTINGS_H_
