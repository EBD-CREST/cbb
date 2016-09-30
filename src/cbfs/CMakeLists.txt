find_package (FUSE REQUIRED)
find_package (OpenSSL REQUIRED)
find_package (Boost COMPONENTS system REQUIRED)

set(CMAKE_C_FLAGS "${FUSE_DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${FUSE_DEFINITIONS}")

include_directories (
  ${FUSE_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src
  )

link_directories (
  ${Boost_LIBRARY_DIRS}
  )

add_library (
  cbb_client_wrapper
  cbb_client_wrapper.h
  cbb_client_wrapper.cc
  )

target_link_libraries (
  cbb_client_wrapper
  cbb_client
  cbb_util
  msgpack
  boost_system
  )
  
add_executable (
  cbfs
  cbfs.cc
  )

target_link_libraries (
  cbfs
  cbb_client_wrapper
  stdc++
  ${FUSE_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  )

install (TARGETS cbfs DESTINATION bin)
install (TARGETS cbb_client_wrapper DESTINATION lib)
