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
#include "util/file_control.h"

#define TEMP_FILE TEST_WORKSPACE"/test_file.tmp"

// ファイル制御クラスユニットテスト

BOOST_AUTO_TEST_SUITE_EX(file_control)

BOOST_AUTO_TEST_CASE(create)
{
  remove(TEMP_FILE);

  cbb::FileControl fc;
  int fd = fc.Create(TEMP_FILE, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);

  BOOST_CHECK(fd != -1);

  fc.Close();
}

BOOST_AUTO_TEST_CASE(open)
{
  cbb::FileControl fc;
  int fd = fc.Open(TEMP_FILE, O_WRONLY);

  BOOST_CHECK(fd != -1);

  fc.Close();
}

BOOST_AUTO_TEST_CASE(write)
{
  cbb::FileControl fc;
  int fd = fc.Open(TEMP_FILE, O_WRONLY);

  BOOST_CHECK(fd != -1);

  char *msg = (char *)"test";
  ssize_t size = fc.Write(msg, 4, 0);

  BOOST_CHECK(size > 0);

  fc.Close();
}

BOOST_AUTO_TEST_CASE(read)
{
  cbb::FileControl fc;
  int fd = fc.Open(TEMP_FILE, O_RDONLY);

  BOOST_CHECK(fd != -1);

  char buff[16] = "";
  ssize_t size = fc.Read(buff, 2, 0);

  BOOST_CHECK(size > 0);

  buff[2] = NULL;
  TMSG("read = %s\n", buff);

  fc.Close();

  remove(TEMP_FILE);
}

BOOST_AUTO_TEST_SUITE_END()
