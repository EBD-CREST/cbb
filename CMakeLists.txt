project (cbb)
cmake_minimum_required (VERSION 2.8.12)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/misc/cmake/modules)

add_subdirectory (src)
add_subdirectory (tests/cases)
