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
#include "burst_buffer.h"
#include "util/options.h"
#include "util/settings.h"

// CBBモジュールmain関数処理

#define CBB_DESC \
  "Usage: %s [options]\n" \
  "  --option=PATH          load setting file path\n"

static msgpack::rpc::server *g_server = NULL;

/**
 * @breaf signal handler
 * @param sig signal
 */
static void signal_handler(int sig) {
  DMSG("signal_handler : %d\n", sig);
  if (g_server != NULL) {
    g_server->end();
    g_server->close();
  }
}

/**
 * @breaf CBBメイン
 * @param argc 引数個数
 * @param argv 引数値
 * @return 処理結果
 */
int main(int argc, char *argv[]) {
  // コマンドライン解析
  cbb::Options options(argc, argv, cbb::S(CBB_DESC, argv[0]));
  cbb::Settings settings;

  if (options.is_help()) {
    printf("%s\n", options.option_desc().c_str());
    return 0;
  }

  // 設定ファイル読み込み
  if (!settings.Load(options.option_dir().c_str(), true)) {
    printf("setting file load error : %s\n", options.option_dir().c_str());
    return -1;
  }

  DMSG("local strage = %s\n", settings.server_local_strage_path().c_str());
  DMSG("secondary strage = %s\n", settings.server_secondary_storage_path().c_str());
  DMSG("host = %s\n", settings.server_host().c_str());
  DMSG("port = %d\n", settings.server_port());
  DMSG("thread = %d\n", settings.server_thread());
  DMSG("interval time = %d min\n", settings.server_interval_time());
  DMSG("------------------------\n");

  // signal設定
  signal(SIGHUP, signal_handler);
  signal(SIGINT, signal_handler);
  signal(SIGABRT, signal_handler);
  signal(SIGKILL, signal_handler);

  // BurstBuffer構築・MsgPack設定
  cbb::BurstBuffer bb(settings.server_local_strage_path(), settings.server_secondary_storage_path(), settings.server_interval_time());
  g_server = &bb.instance;
  bb.instance.listen(settings.server_host(), settings.server_port());
  bb.instance.run(settings.server_thread()); // run 1 threads
}
