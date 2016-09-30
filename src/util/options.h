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
#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>
#include <boost/program_options.hpp>

namespace cbb {

// オプション制御クラス
class Options {
 public:
  Options(int argc, char *argv[], const char *desc);
  virtual ~Options() {}

  bool is_help() { return is_help_; }
  std::string option_dir() { return option_dir_; }
  std::string option_desc() { return option_desc_; }

 private:
  bool is_help_;
  std::string option_dir_;
  std::string option_desc_;
};

} // namesapce cbb

#endif // OPTIONS_H_
