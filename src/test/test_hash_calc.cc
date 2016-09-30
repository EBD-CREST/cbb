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
#include "util/hash/hash_calc_md5.h"

// ハッシュ計算クラスユニットテスト

BOOST_AUTO_TEST_SUITE_EX(hash_calc)

BOOST_AUTO_TEST_CASE(md5)
{
  std::string str = "VigX3kF5h1Vs077fR37kzPofoPqmyDBHWBaRhmJAcmSXTlDO16ttqsNdMZ3n7WgWqCX5PccBJ4WRFlrliUhmuUKalQ3ynBAMrAR2OPFDApbG10SxCi7SYEnbbyd4ISEl";
  cbb::HashCalcMD5 md5;
  boost::multiprecision::int256_t md5sum("0x70742079c9d123a8a0c293b27acba834");
  boost::multiprecision::int256_t md5calc = md5.CalcHash(str.c_str(), str.length());

  BOOST_CHECK(md5sum == md5calc);
}

BOOST_AUTO_TEST_SUITE_END()
