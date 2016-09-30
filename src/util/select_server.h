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
#ifndef UTIL_SELECT_SERVER_H_
#define UTIL_SELECT_SERVER_H_

#include <list>
#include "util/settings.h"
#include "util/hash/hash_calc_base.h"
#include "util/hash/consistent_hash.h"

namespace cbb {

struct ServerInfo {
  std::string host;
  int port;

  ServerInfo() {
    host = "";
    port = 0;
  }

  ServerInfo(const char *h, int p) {
    host = h;
    port = p;
  }
};

// サーバー選択クラス
class SelectServer {
 public:
  SelectServer() : hash_calc_(NULL) {}
  virtual ~SelectServer() {}

  void Init(Settings &settings, HashCalcBase *hash_calc);
  void GetInfo(const char *path, std::string &host, int &port);

  std::list<ServerInfo> server_list() { return server_list_; }

 private:
  HashCalcBase *hash_calc_;
  ConsistentHash<ServerInfo> chash_;
  std::list<ServerInfo> server_list_;
};

} // namespace cbb

#endif /* UTIL_SELECT_SERVER_H_ */
