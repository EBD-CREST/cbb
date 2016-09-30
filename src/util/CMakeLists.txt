find_package (JubatusMPIO)
find_package (JubatusMsgPackRPC)
find_package (Boost COMPONENTS regex system filesystem program_options REQUIRED)
#find_package (TBB)

include_directories (
  ${PROJECT_SOURCE_DIR}/src
  ${JUBATUS_MPIO_INCLUDE_DIR}
  ${JUBATUS_MSGPACK_RPC_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  )

link_directories (
  ${Boost_LIBRARY_DIRS}
  )

add_library (
  cbb_util
  options.h
  options.cc
  settings.h
  settings.cc
  select_server.h
  select_server.cc
  mutex.h
  mutex.cc
  file_control.h
  file_control.cc
  mutex_file.h
  mutex_file.cc
  thread.h
  thread.cc
  hash/hash_calc_base.h
  hash/hash_calc_md5.h
  hash/hash_calc_md5.cc
  hash/hash_calc_sha1.h
  hash/hash_calc_sha1.cc
  hash/consistent_hash.h
  )

target_link_libraries (
  cbb_util
  ${JUBATUS_MPIO_LIBRARIES}
  ${JUBATUS_MSGPACK_RPC_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  )

install (TARGETS cbb_util DESTINATION lib)
