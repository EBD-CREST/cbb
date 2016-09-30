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
#include "hash_calc_sha1.h"

#include <stdio.h>
#include <boost/uuid/sha1.hpp>

// SHA1ハッシュ計算クラス
namespace cbb {

/**
 * @see HashCalcBase::GetKeyBits()
 */
int HashCalcSHA1::GetKeyBits() {
  return 160;
}

/**
 * @see HashCalcBase::CalcHash()
 */
boost::multiprecision::int256_t HashCalcSHA1::CalcHash(const char *str, int len) {
  boost::multiprecision::int256_t rethash = 0;
  boost::uuids::detail::sha1 sha1;
  char hash[20];
  unsigned int digest[5];

  sha1.process_bytes(str, len);
  sha1.get_digest(digest);

  for (int i = 0; i < 5; ++i) {
    const char* tmp = reinterpret_cast<char*>(digest);
    hash[i * 4] = tmp[i * 4 + 3];
    hash[i * 4 + 1] = tmp[i * 4 + 2];
    hash[i * 4 + 2] = tmp[i * 4 + 1];
    hash[i * 4 + 3] = tmp[i * 4];
  }

  for(int index = 0; index < 20; index++) {
    rethash <<= 8;
    rethash += (int)hash[index];
  }

  return rethash;
}

} // namespace cbb
