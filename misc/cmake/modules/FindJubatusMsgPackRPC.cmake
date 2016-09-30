#
# Find JubatusMsgPackRPC library
#

find_library (
  JUBATUS_MSGPACK_RPC_LIBRARIES
  NAMES jubatus_msgpack-rpc
  HINTS "$ENV{JUBATUS_MSGPACK_RPC_DIR}/lib"
  PATHS ENV LD_LIBRARY_PATH
  )

find_path (
  JUBATUS_MSGPACK_RPC_INCLUDE_DIR
  NAMES jubatus/msgpack/rpc/server.h 
  HINTS "$ENV{JUBATUS_MSGPACK_RPC_DIR}/include"
  )

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (
  jubatus_msgpack-rpc
  DEFAULT_MSG
  JUBATUS_MSGPACK_RPC_INCLUDE_DIR
  JUBATUS_MSGPACK_RPC_LIBRARIES
  )

mark_as_advanced (
  JUBATUS_MSGPACK_RPC_INCLUDE_DIR
  JUBATUS_MSGPACK_RPC_LIBRARIES
  )


