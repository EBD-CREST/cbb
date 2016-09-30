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
#ifndef UTIL_HASH_CONSISTENT_HASH_H_
#define UTIL_HASH_CONSISTENT_HASH_H_

#include <vector>
#include <boost/multiprecision/cpp_int.hpp>

// コンシステントハッシュ共通ヘッダー
namespace cbb {

/**
 * ConsistentHash
 */
template<typename T>
class ConsistentHash {
 public:
  struct NodeCircleInfo {
    boost::multiprecision::int256_t value;
    T info;
  };

 public:
  ConsistentHash() {}
  virtual ~ConsistentHash() {}

  /**
   * @breaf ConsistentHash構築
   * @param key_bits Hashキーのビット数
   * @param infos Nodeに保存する情報
   */
  void Create(int key_bits, std::vector<T> infos) {
    int count = infos.size();
    std::vector<boost::multiprecision::int256_t> node_point_value = CalcNodePointValue(key_bits, count);

    // NodeCircle作成
    node_circle_.clear();
    for (int node = 0; node < count; node++) {
      NodeCircleInfo info = { node_point_value[node], infos[node] };
      node_circle_.push_back(info);
    }
  }

  /**
   * @breaf KeyからNodeを取得する
   * @param key 検索するKey
   * @return T Nodeに保存されている情報
   */
  T GetNode(boost::multiprecision::int256_t key) {
    if (node_circle_.empty()) {
      return empty_;
    }

    // 指定されたKeyが所属するNodeを探す
    int max_index = node_circle_.size();
    for (int index = 0; index < max_index; index++) {
      if (key <= node_circle_[index].value) {
        return node_circle_[index].info;
      }
    }

    return empty_;
  }

 private:
  /**
   * @breaf NodeのKey判定値を計算する
   * @param key_bits Hashキーのビット数
   * @param node_count Node数
   * @return std::vector<boost::multiprecision::int256_t> Nodeの判定値
   */
  std::vector<boost::multiprecision::int256_t> CalcNodePointValue(int key_bits, int node_count) {
    std::vector<boost::multiprecision::int256_t> point_value;
    boost::multiprecision::int256_t max_value = 1;
    boost::multiprecision::int256_t add_value = 0;
    boost::multiprecision::int256_t value = 0;

    // Node値を計算（設定）する
    max_value <<= key_bits;
    max_value -= 1;
    add_value = max_value / node_count;

    // サーバー数-1まで設定する
    for (int index = 0; index < node_count - 1; index++) {
      value += add_value;
      point_value.push_back(value);
    }

    // 最終ポイントに最大値を設定する
    point_value.push_back(max_value);

    return point_value;
  }

  std::vector<NodeCircleInfo> node_circle_;
  T empty_;
};

} // namespace cbb

#endif /* UTIL_HASH_CONSISTENT_HASH_H_ */
