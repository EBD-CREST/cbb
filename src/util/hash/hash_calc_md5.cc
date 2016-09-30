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
#include "hash_calc_md5.h"

#include <stdio.h>
#include <openssl/md5.h>

// MD5ハッシュ計算クラス
namespace cbb {

/**
 * @see HashCalcBase::GetKeyBits()
 */
int HashCalcMD5::GetKeyBits() {
  return 128;
}

/**
 * @see HashCalcBase::CalcHash()
 */
boost::multiprecision::int256_t HashCalcMD5::CalcHash(const char *str, int len) {
  boost::multiprecision::int256_t rethash = 0;
  MD5_CTX context;
  unsigned char digest[MD5_DIGEST_LENGTH];
  int ret;

  ret = MD5_Init(&context);
  if (ret) {
    ret = MD5_Update(&context, str, len);
    if (ret) {
      ret = MD5_Final(digest, &context);
      if (ret) {
        for(int index = 0; index < 16; index++) {
          rethash <<= 8;
          rethash += (int)digest[index];
        }
      }
    }
  }

  return rethash;
}

} // namespace cbb
