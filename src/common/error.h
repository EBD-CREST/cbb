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
#ifndef CBB_ERROR_H_
#define CBB_ERROR_H_

#include <stdio.h>
#include <cerrno>
#include <cassert>
#include <boost/format.hpp>

// エラー処理ヘッダー
namespace cbb {

enum CBBErrors {
  kCBBSuccess = 0,
  kCBBUnknownError = 1,
};

/*
 *  Error Definition:
 *
 *  if (error == 0)
 *     Sucess
 *  else if (error > 0)
 *     Return CBBErrors
 *  else if (error < 0)
 *     Return -errno (as the same as FUSE error)
 */
typedef int Error;
/*
inline Error errno_to_cbb_error(int result) {
  Error error = kCBBSuccess;
  if (result == -1) {
    perror(__func__);
    error = -errno;
  }
  return error;
}
*/

static Error tmp_result;
#define errno_to_cbb_error(result)  ((tmp_result = (int)(result)) < 0) ? (perror((boost::format("%1%:%2%>%3%(%4%:%5%)") % __FILE__ % __LINE__ % __FUNCTION__ % errno % tmp_result).str().c_str()), -errno): tmp_result

} // namespace cbb

#endif // CBB_ERROR_H_
