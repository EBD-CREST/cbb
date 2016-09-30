# Try to find fuse (devel)
# Once done, this will define
#
# FUSE_FOUND - system has fuse
# FUSE_INCLUDE_DIR - the fuse include directories
# FUSE_LIBRARIES - fuse libraries directories

if(FUSE_INCLUDE_DIR AND FUSE_LIBRARY)
set(FUSE_FIND_QUIETLY TRUE)
endif(FUSE_INCLUDE_DIR AND FUSE_LIBRARY)

find_path(FUSE_INCLUDE_DIR fuse/fuse_lowlevel.h)
find_library(FUSE_LIBRARY fuse)

set(FUSE_LIBRARIES ${FUSE_LIBRARY})

# for ia64, amd64, x86_64, ia64   CMAKE_SYSTEM_PROCESSOR MATCHIES
set(FUSE_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")

# handle the QUIETLY and REQUIRED arguments and set FUSE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  fuse DEFAULT_MSG
  FUSE_INCLUDE_DIR
  FUSE_LIBRARIES
  FUSE_DEFINITIONS
  )

mark_as_advanced(
  FUSE_INCLUDE_DIR
  FUSE_LIBRARIES
  FUSE_DEFINITIONS
  )
