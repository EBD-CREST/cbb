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
#include "test_common.h"
#include "util/hash/consistent_hash.h"
#include "util/hash/hash_calc_md5.h"
#include "util/select_server.h"

// コンシステントハッシュクラスユニットテスト

BOOST_AUTO_TEST_SUITE_EX(consistent_hash)

cbb::ConsistentHash<cbb::ServerInfo> ch;
cbb::HashCalcMD5 md5;

BOOST_AUTO_TEST_CASE(create)
{
  std::vector<cbb::ServerInfo> infos;
  infos.push_back(cbb::ServerInfo("192.168.11.121", 9091));
  infos.push_back(cbb::ServerInfo("192.168.11.122", 9091));
  infos.push_back(cbb::ServerInfo("192.168.11.123", 9091));

  ch.Create(md5.GetKeyBits(), infos);

  for (int loop = 0; loop < 10; loop++) {
    char path[256];
    sprintf(path, "/cbb/test/dummy%02ld.bin", loop);
    cbb::ServerInfo info = ch.GetNode(md5.CalcHash(path, strlen(path)));

    BOOST_CHECK(!info.host.empty());
    BOOST_CHECK(info.port > 0);

//    printf("host = %s : port = %d : path = %s\n", info.host.c_str(), info.port, path);
  }
}

BOOST_AUTO_TEST_SUITE_END()
