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
#include "settings.h"

#include "common/error.h"
#include "common/common.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

// 設定ファイル読み込みクラス
namespace cbb {

/**
 * @breaf STL Array 変換
 * @param str 文字列
 * @return std::vector<T> vector
 */
template<typename T>
std::vector<T> to_array(const std::string& str) {
  std::vector<T> result;
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, ',')) {
    result.push_back(boost::lexical_cast<T>(item));
  }
  return result;
}

/**
 * @breaf constractor
 * @param filename ファイル名
 */
bool Settings::Load(const char *filename, bool is_server) {
  bool result = false;
  namespace pt = boost::property_tree;

  if (is_server) {
    client_hosts_.clear();
    client_port_ = 0;

    // Server setting
    try {
      pt::ptree tree;
      pt::read_ini(filename, tree);

      server_host_ = tree.get<std::string>("Server.host");
      server_port_ = tree.get<int>("Server.port");
      server_thread_ = tree.get<int>("Server.thread");
      server_local_strage_path_ = tree.get<std::string>("Server.local_strage_path");
      server_secondary_storage_path_ = tree.get<std::string>("Server.secondary_storage_path");
      server_interval_time_ = tree.get<int>("Server.secondary_storage_path", 1);

      result = true;
    } catch (...) {
      server_host_.clear();
      server_port_ = 0;
      server_thread_ = 0;
      server_local_strage_path_.clear();
      server_secondary_storage_path_.clear();
      server_interval_time_ = 1;
    }

  } else {
    server_host_.clear();
    server_port_ = 0;
    server_thread_ = 0;
    server_local_strage_path_.clear();
    server_secondary_storage_path_.clear();

    // Client setting
    try {
      pt::ptree tree;
      pt::read_ini(filename, tree);

      client_hosts_ = to_array<std::string>(tree.get<std::string>("Client.host"));
      client_port_ = tree.get<int>("Client.port");

      result = true;
    } catch (...) {
      client_hosts_.clear();
      client_port_ = 0;
    }
  }

  return result;
}

} // namesapce cbb
