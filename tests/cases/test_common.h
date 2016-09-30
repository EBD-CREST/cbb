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
#ifndef CBB_TESTS_CASES_COMMON_H_
#define CBB_TESTS_CASES_COMMON_H_

#include "../src/common/common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/statfs.h>
#include <sys/xattr.h>
#include <dirent.h>

#define TEST_DATA_01    "test_data_01.txt" // file
#define TEST_DATA_02    "test_data_02" // dir
#define TEST_DATA_03    "test_data_03.txt" // symlink file
#define TEST_DATA_04    "test_data_04" // symlink dir
#define TEST_DATA_05    "test_data_05" // dir
#define TEST_DATA_06    "test_data_06.txt" // file
#define TEST_DATA_07    "test_data_07.txt" // symlink file
#define TEST_DATA_08    "test_data_08" // symlink dir
#define TEST_DATA_09    "test_data_09.txt" // file
#define TEST_DATA_10    "test_data_10" // dir
#define TEST_DATA_11    "test_data_11.txt" // link file
#define TEST_DATA_12    "test_data_12" // link dir
#define TEST_DATA_13    "test_data_13.txt" // file

#define TEST_DATA_21    "test_data_21.txt" // file
#define TEST_DATA_22    "test_data_22.txt" // file
#define TEST_DATA_23    "test_data_23.txt" // file
#define TEST_DATA_24    "test_data_24.txt" // file
#define TEST_DATA_25    "test_data_25.txt" // file
#define TEST_DATA_26    "test_data_26.txt" // file
#define TEST_DATA_27    "test_data_27.txt" // file
#define TEST_DATA_28    "test_data_28" // dir
#define TEST_DATA_29    "test_data_29.txt" // file
#define TEST_DATA_30    "test_data_30.txt" // file
#define TEST_DATA_31    "test_data_31.txt" // file symlink
#define TEST_DATA_32    "test_data_32" // dir symlink
#define TEST_DATA_33    "test_data_33.txt" // file link
#define TEST_DATA_34    "test_data_34" // dir link


#define TEST2_DATA_01    "test2_data_01.txt" // file
#define TEST2_DATA_01a   "test2_data_01a.txt" // file
#define TEST2_DATA_02    "test2_data_02" // dir
#define TEST2_DATA_02a   "test2_data_02a" // dir
#define TEST2_DATA_03    "test2_data_03.txt" // symlink file
#define TEST2_DATA_03b   "test2_data_03b.txt" // symlink file
#define TEST2_DATA_04    "test2_data_04" // symlink dir
#define TEST2_DATA_04a   "test2_data_04a" // symlink dir
#define TEST2_DATA_05    "test2_data_05" // dir
#define TEST2_DATA_05a   "test2_data_05a" // dir
#define TEST2_DATA_06    "test2_data_06.txt" // file
#define TEST2_DATA_06a   "test2_data_06a.txt" // file
#define TEST2_DATA_07    "test2_data_07.txt" // symlink file
#define TEST2_DATA_07d   "test2_data_07d.txt" // symlink file
#define TEST2_DATA_08    "test2_data_08" // symlink dir
#define TEST2_DATA_08b   "test2_data_08b" // symlink dir
#define TEST2_DATA_09    "test2_data_09.txt" // file
#define TEST2_DATA_09a   "test2_data_09a.txt" // file
#define TEST2_DATA_10    "test2_data_10" // dir
#define TEST2_DATA_10a   "test2_data_10a" // dir
#define TEST2_DATA_11    "test2_data_11.txt" // link file
#define TEST2_DATA_11b   "test2_data_11b.txt" // link file
#define TEST2_DATA_12    "test2_data_12" // link dir
#define TEST2_DATA_12a   "test2_data_12a" // link dir
#define TEST2_DATA_13    "test2_data_13.txt" // file
#define TEST2_DATA_13d   "test2_data_13d.txt" // file

#define TEST2_DATA_21    "test2_data_21.txt" // file
#define TEST2_DATA_21c   "test2_data_21c.txt" // file
#define TEST2_DATA_22    "test2_data_22.txt" // file
#define TEST2_DATA_22b   "test2_data_22b.txt" // file
#define TEST2_DATA_23    "test2_data_23.txt" // file
#define TEST2_DATA_23b   "test2_data_23b.txt" // file
#define TEST2_DATA_24    "test2_data_24.txt" // file
#define TEST2_DATA_24a   "test2_data_24a.txt" // file
#define TEST2_DATA_25    "test2_data_25.txt" // file
#define TEST2_DATA_25e   "test2_data_25e.txt" // file
#define TEST2_DATA_26    "test2_data_26.txt" // file
#define TEST2_DATA_26c   "test2_data_26c.txt" // file
#define TEST2_DATA_27    "test2_data_27.txt" // file
#define TEST2_DATA_27e   "test2_data_27e.txt" // file
#define TEST2_DATA_28    "test2_data_28" // dir
#define TEST2_DATA_28a   "test2_data_28a" // dir
#define TEST2_DATA_29    "test2_data_29.txt" // file
#define TEST2_DATA_29c   "test2_data_29c.txt" // file
#define TEST2_DATA_30    "test2_data_30.txt" // file
#define TEST2_DATA_30b   "test2_data_30b.txt" // file
#define TEST2_DATA_31    "test2_data_31.txt" // file symlink
#define TEST2_DATA_31a   "test2_data_31a.txt" // file symlink
#define TEST2_DATA_32    "test2_data_32" // dir symlink
#define TEST2_DATA_32a   "test2_data_32a" // dir symlink
#define TEST2_DATA_33    "test2_data_33.txt" // file link
#define TEST2_DATA_33a   "test2_data_33a.txt" // file link
#define TEST2_DATA_34    "test2_data_34" // dir link
#define TEST2_DATA_34a   "test2_data_34a" // dir link
#define TEST2_DATA_35    "test2_data_35.txt" // file
#define TEST2_DATA_36    "test2_data_36" // dir


int static output_test_results(const char *test_name, const char *test_case, const char *test_pattern, int result) {
  printf("test case\t%s\t%s\t%s\t%s(%d)\n", test_name, test_case, test_pattern, result<0?"false":"true", result);
  return result;
}

void static time_check(bool is_start) {
  static struct timeval st, ed;
  if (is_start) {
    gettimeofday(&st, NULL);
  } else {
    gettimeofday(&ed, NULL);
    printf("run time\t%ld\tms\n", (ed.tv_sec - st.tv_sec) * 1000 + (ed.tv_usec - st.tv_usec) / 1000);
  }
}


#endif // CBB_TESTS_CASES_COMMON_H_
