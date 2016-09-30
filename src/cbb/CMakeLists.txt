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
  cbb_client
  burst_buffer_client.h
  burst_buffer_client.cc
  )

target_link_libraries (
  cbb_client
  cbb_util
  ${JUBATUS_MPIO_LIBRARIES}
  ${JUBATUS_MSGPACK_RPC_LIBRARIES}
  boost_filesystem
  boost_program_options
  )

add_executable (
  cbb
  cbb.cc
  burst_buffer.h
  burst_buffer.cc
  meta_data_manager.h
  meta_data_manager.cc
  local_file_exporter.h
  local_file_exporter.cc
  )

target_link_libraries (
  cbb
  cbb_util
  ${JUBATUS_MPIO_LIBRARIES}
  ${JUBATUS_MSGPACK_RPC_LIBRARIES}
  msgpack
  boost_system
  boost_filesystem
  boost_program_options
  )

install (TARGETS cbb DESTINATION bin)
install (TARGETS cbb_client DESTINATION lib)

