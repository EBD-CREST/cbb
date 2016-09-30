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
#include "util/options.h"

// オプション制御クラスユニットテスト

BOOST_AUTO_TEST_SUITE_EX(options)

BOOST_AUTO_TEST_CASE(help)
{
  int argc = 2;
  char *argv[] = { (char *)"test_options", (char *)"--help" };
  cbb::Options options(argc, argv, "test_options");

  BOOST_CHECK(options.is_help() == true);
//  printf("help = %d\n", (int)options.is_help());
//  printf("option dir = %s\n", options.option_dir().c_str());
}

BOOST_AUTO_TEST_CASE(option)
{
  int argc = 2;
  char *argv[] = { (char *)"test_options", (char *)"--option=/tmp/dummy" };
  cbb::Options options(argc, argv, "test_options");

  BOOST_CHECK(options.option_dir().empty() == false);
//  printf("help = %d\n", (int)options.is_help());
//  printf("option dir = %s\n", options.option_dir().c_str());
}

BOOST_AUTO_TEST_CASE(none)
{
  int argc = 2;
  char *argv[] = { (char *)"test_options", (char *)"--test" };
  cbb::Options options(argc, argv, "test_options");

  BOOST_CHECK(options.is_help() == true);
//  printf("help = %d\n", (int)options.is_help());
//  printf("option dir = %s\n", options.option_dir().c_str());
}

BOOST_AUTO_TEST_SUITE_END()
