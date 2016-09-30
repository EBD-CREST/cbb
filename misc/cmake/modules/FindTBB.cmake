#
# Find TBB library
#

find_library (
  TBB_LIBRARY
  NAMES tbb
  HINTS "$ENV{TBB_DIR}/lib"
  PATHS ENV LD_LIBRARY_PATH
  )

find_library (
  TBB_MALLOC_LIBRARY
  NAME tbbmalloc
  HINTS "$ENV{TBB_DIR}/lib"
  PATHS ENV LD_LIBRARY_PATH
  )

find_path (
  TBB_INCLUDE_DIR
  NAMES tbb/tbb.h
  HINTS "$ENV{TBB_DIR}/include"
  )

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (
  tbb
  DEFAULT_MSG
  TBB_INCLUDE_DIR
  TBB_LIBRARY
  TBB_MALLOC_LIBRARY
  )

mark_as_advanced (
  TBB_INCLUDE_DIR
  TBB_LIBRARY
  TBB_MALLOC_LIBRARY
  )