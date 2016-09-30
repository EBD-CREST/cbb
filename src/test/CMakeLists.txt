find_package (FUSE REQUIRED)
find_package (OpenSSL REQUIRED)
find_package (Boost COMPONENTS regex system filesystem program_options unit_test_framework REQUIRED)

set(CMAKE_C_FLAGS "${FUSE_DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${FUSE_DEFINITIONS}")

include_directories (
  ${FUSE_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${JUBATUS_MPIO_INCLUDE_DIR}
  ${JUBATUS_MSGPACK_RPC_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src
  )

link_directories (
  ${Boost_LIBRARY_DIRS}
  )

add_definitions (
  -DBOOST_TEST_DYN_LINK
  )

add_executable (
  cbb_test
  test_common.h
  test_main.cc
  test_settings.cc
  test_hash_calc.cc
  test_consistent_hash.cc
  test_select_server.cc
  test_file_control.cc
  test_options.cc
  test_mutex.cc
  )

target_link_libraries (
  cbb_test
  cbb_client
  cbb_util
  stdc++
  ${OPENSSL_LIBRARIES}
  boost_system
  boost_unit_test_framework
  )

install (TARGETS cbb_test DESTINATION bin)
