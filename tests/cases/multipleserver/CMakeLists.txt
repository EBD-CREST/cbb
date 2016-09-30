find_package (Boost COMPONENTS regex system filesystem program_options unit_test_framework REQUIRED)

set(CMAKE_C_FLAGS "${FUSE_DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${FUSE_DEFINITIONS}")

include_directories (
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src
  )

link_directories (
  ${Boost_LIBRARY_DIRS}
  )
  
#add_library (
#  )

#################################################################
set(test_case 0101)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0102)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0103)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0104)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0105)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0106)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0107)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0108)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0109)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0110)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0111)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0112)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0115)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0118)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0119)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0120)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0121)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0122)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0123)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0125)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0126)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0128)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)



#################################################################
set(test_case 0201)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0202)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0203)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0204)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0205)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0206)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0207)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0208)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0209)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0210)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0211)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0212)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0213)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0214)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0215)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0216)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0217)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

#################################################################
set(test_case 0218)
add_executable        (test2_${test_case} test2_${test_case}.cc)
target_link_libraries (test2_${test_case} stdc++ boost_filesystem)
install       (TARGETS test2_${test_case} DESTINATION bin)

