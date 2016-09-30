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
#ifndef TEST_TEST_COMMON_H_
#define TEST_TEST_COMMON_H_

#include "common/common.h"
#include <boost/test/unit_test.hpp>

#define TEST_WORKSPACE "/root/git/cbb/work"

// ユニットテスト共通ヘッダー
class BoostTestSuiteMessager {
 public:
  BoostTestSuiteMessager(const char *msg) {
    printf("%s\n", msg);
  }
};

#define BOOST_AUTO_TEST_SUITE_EX(suite) \
  BoostTestSuiteMessager BTSM_##suite ("testing... ["#suite"]"); \
  BOOST_AUTO_TEST_SUITE(suite)


#endif /* TEST_TEST_COMMON_H_ */
