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

#define TEST_NAME           "Create/Close/Unlink"
#define TEST_CASE           "02-07"
#define TEST_CASE_PATTERN1  "01-01" // FILE/DIR - SUCCESSED/FAILED
#define TEST_CASE_PATTERN2  "01-02" // FILE/DIR - SUCCESSED/FAILED
#define TEST_CASE_PATTERN3  "02-01" // FILE/DIR - SUCCESSED/FAILED
#define TEST_CASE_PATTERN4  "02-02" // FILE/DIR - SUCCESSED/FAILED

// test code
int main() {
  int ret1, ret2, ret3;
  mode_t mode = S_IRWXU;

  ret1 = creat(TEST_DATA_23, mode);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret1);
  ret2 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret2);
  ret3 = unlink(TEST_DATA_23);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret3);

  ret1 = creat("aaa/"TEST_DATA_23, mode);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret1);
  ret2 = close(ret1);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret2);
  ret3 = unlink("aaa/"TEST_DATA_23);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret3);

  return 0;
}
