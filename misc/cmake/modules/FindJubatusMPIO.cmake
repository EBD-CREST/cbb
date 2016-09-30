#
# Find JubatusMPIO library
#

find_library (
  JUBATUS_MPIO_LIBRARIES
  NAMES jubatus_mpio
  HINTS "$ENV{JUBATUS_MPIO_DIR}/lib"
  PATHS ENV LD_LIBRARY_PATH
  )

find_path (
  JUBATUS_MPIO_INCLUDE_DIR
  NAMES jubatus/mp/wavy.h
  HINTS "$ENV{JUBATUS_MPIO_DIR}/include"
  )

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (
  jubatus_mpio
  DEFAULT_MSG
  JUBATUS_MPIO_INCLUDE_DIR
  JUBATUS_MPIO_LIBRARIES
  )

mark_as_advanced (
  JUBATUS_MPIO_INCLUDE_DIR
  JUBATUS_MPIO_LIBRARIES
  )

