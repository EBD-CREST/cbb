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
#include "util/settings.h"

// 設定内容クラスユニットテスト

BOOST_AUTO_TEST_SUITE_EX(settings)

cbb::Settings settings;

BOOST_AUTO_TEST_CASE(load)
{
  BOOST_CHECK(settings.Load(TEST_WORKSPACE"/cbb.conf", true));
  BOOST_CHECK(settings.Load(TEST_WORKSPACE"/cbb.conf", false));
}

BOOST_AUTO_TEST_SUITE_END()
