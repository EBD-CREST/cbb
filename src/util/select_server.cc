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
#include "select_server.h"

#include <boost/foreach.hpp>

#include "util/hash/hash_calc_md5.h"
#include "util/hash/hash_calc_sha1.h"
#include "util/hash/consistent_hash.h"

// サーバー選択クラス
namespace cbb {

/**
 * @breaf 初期化
 * @param settings 設定クラス
 * @param hash_calc Hash計算クラス
 */
void SelectServer::Init(Settings &settings, HashCalcBase *hash_calc) {
  assert(hash_calc != NULL);

  server_list_.clear();

  // Hash計算クラス保存
  hash_calc_ = hash_calc;

  // サーバーのIP、Portを登録
  std::vector<ServerInfo> infos;
  BOOST_FOREACH(std::string host, settings.client_hosts()) {
    ServerInfo info(host.c_str(), settings.client_port());
    infos.push_back(info);
    server_list_.push_back(info);
  }

  // ConsistentHash構築
  chash_.Create(hash_calc_->GetKeyBits(), infos);
}

/**
 * @breaf サーバー情報取得
 * @param path path情報
 * @param host サーバーのhost情報を保存
 * @param port サーバーのport情報を保存
 */
void SelectServer::GetInfo(const char *path, std::string &host, int &port) {
  assert(hash_calc_ != NULL);
  assert(path != NULL);

  host = "";
  port = 0;

  boost::multiprecision::int256_t hash = hash_calc_->CalcHash(path, strlen(path));
  ServerInfo info = chash_.GetNode(hash);

  assert(!info.host.empty());
  assert(info.port > 0);

  host = info.host;
  port = info.port;
}

} // namespace cbb
