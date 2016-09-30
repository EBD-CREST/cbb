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
#ifndef UTIL_HASH_HASH_CALC_BASE_H_
#define UTIL_HASH_HASH_CALC_BASE_H_

#include <boost/multiprecision/cpp_int.hpp>

// ハッシュ計算ベースヘッダー
class HashCalcBase {
 public:
  HashCalcBase() {}
  virtual ~HashCalcBase() {}

  /**
   * @breaf HashのBit数を返す
   * @return int Bit数
   */
  virtual int GetKeyBits() = 0;

  /**
   * @breaf 渡された文字列からHashを計算する
   * @param str Hash計算する文字列
   * @param len 文字列長
   * @return boost::multiprecision::int256_t Hash値
   */
  virtual boost::multiprecision::int256_t CalcHash(const char *str, int len) = 0;
};

#endif /* UTIL_HASH_HASH_CALC_BASE_H_ */
