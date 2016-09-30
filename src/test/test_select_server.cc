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

#include <boost/foreach.hpp>

#include "util/settings.h"
#include "util/select_server.h"
#include "util/hash/hash_calc_md5.h"
#include "util/hash/hash_calc_sha1.h"

// サーバー選択クラスユニットテスト

BOOST_AUTO_TEST_SUITE_EX(select_server)

cbb::Settings settings;
cbb::SelectServer ss;

BOOST_AUTO_TEST_CASE(create)
{
  BOOST_CHECK(settings.Load(TEST_WORKSPACE"/cbb.conf", false));

  BOOST_FOREACH(std::string host, settings.client_hosts()) {
//    printf("host = %s\n", host.c_str());
  }

  //ss.Init(settings, new cbb::HashCalcMD5());
  ss.Init(settings, new cbb::HashCalcSHA1());

  for (int loop = 0; loop < 10; loop++) {
    char path[256];
    std::string host;
    int port;

    sprintf(path, "/cbb/test/dummy%02ld.bin", loop);
    ss.GetInfo(path, host, port);

    BOOST_CHECK(!host.empty());
    BOOST_CHECK(port > 0);

//    printf("host = %s : port = %d : path = %s\n", host.c_str(), port, path);
  }
}

BOOST_AUTO_TEST_SUITE_END()
