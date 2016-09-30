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
#include "../test_common.h"

#define TEST_NAME           "Open/Write/Read/Close"
#define TEST_CASE           "02-06"
#define TEST_CASE_PATTERN1  "01-01" // FILE/DIR - SUCCESSED/FAILED
#define TEST_CASE_PATTERN2  "01-02" // FILE/DIR - SUCCESSED/FAILED
#define TEST_CASE_PATTERN3  "02-01" // FILE/DIR - SUCCESSED/FAILED
#define TEST_CASE_PATTERN4  "02-02" // FILE/DIR - SUCCESSED/FAILED

// test code
int main() {
  int ret1, ret2, ret3, ret4, ret5, ret6;
  int flags1 = O_WRONLY;
  int flags2 = O_RDONLY;
  unsigned char buf[1024] = "Test Data";

  ret1 = open(TEST2_DATA_21, flags1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret1);
  ret2 = write(ret1, buf, sizeof(buf));
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret2);
  ret3 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret3);
  ret4 = open(TEST2_DATA_21, flags2);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret4);
  ret5 = read(ret1, buf, sizeof(buf));
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret5);
  ret6 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret6);

  ret1 = open(TEST2_DATA_21c, flags1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret1);
  ret2 = write(ret1, buf, sizeof(buf));
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret2);
  ret3 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret3);
  ret4 = open(TEST2_DATA_21c, flags2);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret4);
  ret5 = read(ret1, buf, sizeof(buf));
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret5);
  ret6 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret6);

  ret1 = open("aaa/"TEST2_DATA_21, flags1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret1);
  ret2 = write(ret1, buf, sizeof(buf));
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret2);
  ret3 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret3);
  ret4 = open("aaa/"TEST2_DATA_21, flags2);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret4);
  ret5 = read(ret1, buf, sizeof(buf));
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret5);
  ret6 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret6);

  ret1 = open("aaa/"TEST2_DATA_21c, flags1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret1);
  ret2 = write(ret1, buf, sizeof(buf));
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret2);
  ret3 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret3);
  ret4 = open("aaa/"TEST2_DATA_21c, flags2);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret4);
  ret5 = read(ret1, buf, sizeof(buf));
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret5);
  ret6 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret6);

  return 0;
}
