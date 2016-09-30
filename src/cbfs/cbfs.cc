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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <limits.h>

#define FUSE_USE_VERSION 28
#include <fuse.h>

#include "common/common.h"

#include "cbb_client_wrapper.h"

// CBFSモジュールmain処理

static char *g_cbfs_root = NULL;
static char *g_program_name = (char *)"cbfs";

static struct fuse_operations cbfs_operations = { NULL };

/**
 * @breaf FUSE 操作関数設定
 */
static void SetOperations() {
  cbfs_operations.getattr = CBFSGetAttr;
  cbfs_operations.readlink = CBFSReadLink;
  cbfs_operations.mkdir = CBFSMkDir;
  cbfs_operations.unlink = CBFSUnlink;
  cbfs_operations.rmdir = CBFSRmDir;
  cbfs_operations.symlink = CBFSSymlink;
  cbfs_operations.rename = CBFSRename;
  cbfs_operations.link = CBFSLink;
  cbfs_operations.chmod = CBFSChmod;
  cbfs_operations.chown = CBFSChown;
  cbfs_operations.truncate = CBFSTruncate;

  cbfs_operations.open = CBFSOpen;
  cbfs_operations.read = CBFSRead;
  cbfs_operations.write = CBFSWrite;
  cbfs_operations.statfs = CBFSStatFs;
  cbfs_operations.flush = CBFSFlush;
  cbfs_operations.release = CBFSRelease;
  cbfs_operations.fsync = CBFSFSync;

  cbfs_operations.setxattr = CBFSSetXAttr;
  cbfs_operations.getxattr = CBFSGetXAttr;
  cbfs_operations.listxattr = CBFSListXAttr;
  cbfs_operations.removexattr = CBFSRemoveXAttr;

  cbfs_operations.readdir = CBFSReadDir;
  cbfs_operations.fsyncdir = CBFSFSyncDir;

  cbfs_operations.init = CBFSInit;
  cbfs_operations.destroy = CBFSDestroy;

  cbfs_operations.access = CBFSAccess;
  cbfs_operations.create = CBFSCreate;
  cbfs_operations.ftruncate = CBFSFTruncate;
//  cbfs_operations.fgetattr = CBFSFGetAttr;
//  cbfs_operations.lock = CBFSLock;
  cbfs_operations.utimens = CBFSUtimens;
}

/**
 * @breaf 使用方法
 */
static void Usage() {
  const char *fusehelp[] = { g_program_name, "-ho", NULL };

  fprintf(stderr,
	  "Usage: %s [CBFS options] <mountpoint> [FUSE options]\n"
	  "\n"
	  "  --option=PATH          load setting file path"
	  "\n", g_program_name);

  fuse_main(2, (char **) fusehelp, &cbfs_operations, NULL);
}

/**
 * @breaf 絶対パスかどうかを調べる
 * @param path 調べるパス
 * @return 絶対パス = 1 / 相対パス = 0
 */
static int is_absolute_path(const char *path) {
  if (path && path[0] != '\0' && path[0] == '/')
    return 1;
  else 
    return 0;
}

/**
 * @breaf ロングオプション解析
 * @param argcp 引数個数ポインタ
 * @param argvp 引数内容ポインタ
 */
static void ParseLongOptions(int *argcp, char ***argvp) {
  char **argv = *argvp;

  if (!strncmp(&argv[0][2], "option=", 7)) {
    CBFSSetConfigPath((const char *)&argv[0][2 + 7]);
  }
}

/**
 * @breaf オプション解析
 * @param argcp 引数個数ポインタ
 * @param argvp 引数内容ポインタ
 */
static void ParseOptions(int *argcp, char ***argvp) {

  int argc = *argcp;
  char **argv = *argvp;
  char *argv0 = *argv;

  --argc;
  ++argv;
  while (argc > 0 && argv[0][0] == '-') {

    if (argv[0][1] == '-') {
      ParseLongOptions(&argc, &argv);
    } else {
      Usage();
    }

    --argc;
    ++argv;
  }
  ++argc;
  --argv;

  // set absolute path
  if (argc > 1 && argv[1][0] != '-') {
    if (is_absolute_path(argv[1])) { 
      g_cbfs_root = argv[1];
    } else {
      char abs_path[PATH_MAX+1];
      g_cbfs_root = realpath(argv[1], NULL);
      if (g_cbfs_root == NULL) {
        Usage();
      }
      argv[1] = g_cbfs_root;
    }
  }

  *argcp = argc;
  *argv = argv0;
  *argvp = argv;
}

/**
 * @breaf main
 * @param argc 引数個数
 * @param argv 引数内容
 * @return 終了コード
 */
int main(int argc, char *argv[]) {

  if (argc > 0)
    g_program_name = basename(argv[0]);

  CBFSSetConfigPath(CBB_CONFIG);
  SetOperations();

  ParseOptions(&argc, &argv);
  umask(0);

  return fuse_main(argc, argv, &cbfs_operations, NULL);
}
