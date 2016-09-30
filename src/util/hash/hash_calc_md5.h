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
#ifndef SRC_UTIL_HASH_HASH_CALC_MD5_H_
#define SRC_UTIL_HASH_HASH_CALC_MD5_H_

#include "hash_calc_base.h"

namespace cbb {

// MD5ハッシュ計算クラス
class HashCalcMD5 : public HashCalcBase {
 public:
  HashCalcMD5() {}
  virtual ~HashCalcMD5() {}

  virtual int GetKeyBits();
  virtual boost::multiprecision::int256_t CalcHash(const char *str, int len);
};

} // namespace cbb

#endif /* SRC_UTIL_HASH_HASH_CALC_MD5_H_ */
