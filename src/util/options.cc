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
#include "common/error.h"
#include "common/common.h"
#include "options.h"

#define ARG_OPTION  "option"
#define ARG_HELP    "help"

// オプション制御クラス
namespace cbb {

/**
 * @breaf constractor
 * @param argc 引数個数
 * @param argv 引数内容
 * @param desc 説明
 */
Options::Options(int argc, char *argv[], const char *desc) {

  namespace po = boost::program_options;

  po::options_description opt(desc);
  po::variables_map vm;

  opt.add_options()
    (ARG_OPTION,	po::value<std::string>(&option_dir_), "option file")
    (ARG_HELP, "Help")
  ;

  is_help_ = false;

  try {
    po::store(po::parse_command_line(argc, argv, opt), vm);
    po::notify(vm);

    if (!vm.count(ARG_OPTION)) {
      option_dir_ = CBB_CONFIG;
    }
    if (vm.count(ARG_HELP)) {
      is_help_ = true;
      option_desc_ = desc;
    }
  }
  catch(const boost::program_options::error_with_option_name& e) {
    is_help_ = true;
    option_desc_ = desc;
    option_dir_ = CBB_CONFIG;
  }
}

} // namesapce cbb
