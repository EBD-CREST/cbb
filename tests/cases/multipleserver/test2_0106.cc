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

#define TEST_NAME           "SymLink"
#define TEST_CASE           "01-06"
#define TEST_CASE_PATTERN1  "01-01" // FILE/DIR - SUCCESSED/FAILED
#define TEST_CASE_PATTERN2  "01-02" // FILE/DIR - SUCCESSED/FAILED
#define TEST_CASE_PATTERN3  "02-01" // FILE/DIR - SUCCESSED/FAILED
#define TEST_CASE_PATTERN4  "02-02" // FILE/DIR - SUCCESSED/FAILED

// test code
int main() {
  int ret;

  ret = symlink(TEST2_DATA_01, TEST2_DATA_07);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret);
  ret = symlink(TEST2_DATA_01, TEST2_DATA_07d);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN1, ret);

  ret = symlink(TEST2_DATA_01, TEST2_DATA_07);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret);
  ret = symlink(TEST2_DATA_01, TEST2_DATA_07d);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN2, ret);

  ret = symlink(TEST2_DATA_02, TEST2_DATA_08);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN3, ret);
  ret = symlink(TEST2_DATA_02, TEST2_DATA_08b);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN3, ret);

  ret = symlink(TEST2_DATA_02, TEST2_DATA_08);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN4, ret);
  ret = symlink(TEST2_DATA_02, TEST2_DATA_08b);
  output_test_results(TEST_NAME, TEST_CASE, TEST_CASE_PATTERN4, ret);

  return 0;
}
