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
#ifndef CBB_COMMON_H_
#define CBB_COMMON_H_

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/statvfs.h>

#include <string>

#include <msgpack.hpp>
#include <boost/filesystem/fstream.hpp>

#ifndef CBB_CONFIG
  #define CBB_CONFIG "/etc/cbb.conf"
#endif

#define OUTPUT_PRINTF_DEBUG
#define OUTPUT_PRINTF_TEST

#define VIRTUAL_SYMLINK_EXT   ".870A7545-5F58-453A-A685-1C9A3A17272C_symlink"
#define VIRTUAL_LINK_EXT      ".2257FC83-FA8A-430F-AB47-9105B352793D_link"

// 共通ヘッダー
namespace cbb {

struct TimeSpec {
  long tv_sec;
  long tv_nsec;

  MSGPACK_DEFINE(tv_sec, tv_nsec);
};

struct FileStat {

  dev_t st_dev;
  ino_t st_ino;
  mode_t st_mode;
  nlink_t   st_nlink;   
  uid_t     st_uid;     
  gid_t     st_gid;   
  dev_t     st_rdev;  
  off_t     st_size;  
  blksize_t st_blksize; 
  
  blkcnt_t  st_blocks;

  TimeSpec st_atim;
  TimeSpec st_mtim;
  TimeSpec st_ctim;
  
  MSGPACK_DEFINE(st_dev,
                 st_ino,
                 st_mode,
                 st_nlink,
                 st_uid,
                 st_gid,
                 st_rdev,
                 st_size,
                 st_blksize,
                 st_blocks,
                 st_atim,
                 st_mtim,
                 st_ctim);
};

struct FLock : public flock {
  MSGPACK_DEFINE(
      l_type,
      l_whence,
      l_start,
      l_len,
      l_pid
  );
};

struct StatVfs : public statvfs {
  MSGPACK_DEFINE(
      f_bsize,
      f_frsize,
      f_blocks,
      f_bfree,
      f_bavail,
      f_files,
      f_ffree,
      f_favail,
      f_fsid,
#ifdef _STATVFSBUF_F_UNUSED
      __f_unused,
#endif
      f_flag,
      f_namemax,
      __f_spare[6]
  );
};

typedef std::map<std::string, cbb::FileStat> FileStats;

/// MsgPack Code
#define CODE(code) #code
enum CBBMsgPackCode {
  kNone = 0,
  kGetAttr,
  kReadLink,
  kMkDir,
  kUnlink,
  kRmDir,
  kSymlink,
  kRename,
  kLink,
  kChmod,
  kChown,
  kTruncate,

  kOpen,
  kRead,
  kWrite,
  kStatFs,
  kFlush,
  kRelease,
  kFSync,

  kSetXAttr,
  kGetXAttr,
  kListXAttr,
  kRemoveXAttr,

  kReadDir,
  kFSyncDir,

  kAccess,
  kCreate,
  kFTruncate,
  kFGetAttr,
  kLock,
  kUtimens,

  kFilePrevRead,
  kFileFlush,
  kLocalFileExport,
};

enum CBBReadDirType {
  kDirLocal = 1,
  kDirSecondary = 2,
  kDirAll = 3,
};

/**
 * 文字列生成クラス
 */
class S
{
private:
  char str[ 512 ];

public:
  /**
   * @breaf constractor
   * @param format フォーマット
   */
  S( const char *format, ... )
  {
    va_list arg;
    va_start( arg, format );
    vsnprintf( str, sizeof(str), format, arg );
    va_end( arg );
    str[ sizeof(str) - 1 ] = NULL;
  }

  operator const char * ()		{ return str; }
};

#ifdef OUTPUT_PRINTF_DEBUG
  #define DMSG(...) printf((const char *)cbb::S(__VA_ARGS__))
#else
  #define DMSG(...)
#endif

#ifdef OUTPUT_PRINTF_TEST
  #define TMSG(...) printf((const char *)cbb::S(__VA_ARGS__))
#else
  #define TMSG(...)
#endif


#define CBB_LOG_FILE "/tmp/cbb.log"
static void FMSG(const char *filename, const char *format, ... )
{
  char  str[512];
  va_list arg;
  va_start( arg, format );
  vsnprintf( str, sizeof(str), format, arg );
  va_end( arg );
  str[ sizeof(str) - 1 ] = NULL;

  FILE *fpw = fopen(filename, "at");
  if( fpw != NULL )
  {
    fprintf(fpw, "%s\n", str);
    fclose( fpw );
  }
}

static uint64_t get_time_msec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000);
}

static std::string create_virtual_symlink(const std::string path) {
  std::string target = path + VIRTUAL_SYMLINK_EXT;
  return target;
}

static bool is_virtual_symlink(const std::string path) {
  return path.find(VIRTUAL_SYMLINK_EXT) != std::string::npos ? true: false;
}

static std::string create_virtual_link(const std::string path) {
  std::string target = path + VIRTUAL_LINK_EXT;
  return target;
}

static bool is_virtual_link(const std::string path) {
  return path.find(VIRTUAL_LINK_EXT) != std::string::npos ? true: false;
}

static std::string remove_virtual_ext(std::string path) {
  std::string::size_type index = path.find(VIRTUAL_SYMLINK_EXT);
  if (index != std::string::npos) {
    path = path.replace(index, strlen(VIRTUAL_SYMLINK_EXT), "");
  } else {
    index = path.find(VIRTUAL_LINK_EXT);
    if (index != std::string::npos) {
      path = path.replace(index, strlen(VIRTUAL_LINK_EXT), "");
    }
  }
  return path;
}

static std::string read_one_line(const std::string path) {
  std::string line;
  boost::filesystem::ifstream ifs(path);
  if( !ifs.fail() ) {
    std::getline(ifs, line);
    ifs.close();
  }
  return line;
}

static void write_one_line(const std::string path, const std::string text) {
  boost::filesystem::ofstream ofs(path);
  ofs << text << std::endl;
  ofs.close();
}


} // namespace cbb

#endif // CBB_COMMON_H_
