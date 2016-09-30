#!/bin/sh

# Make file create
#cmake -DBoost_INCLUDE_DIR=/root/oss/boost_1_58_0/include -DCMAKE\_INSTALL\_PREFIX=/usr/src/cbb ..
cmake -DCMAKE\_INSTALL\_PREFIX=/usr/src/cbb ..

# Build & Install
#make clean
make
make install

# Unit Test
cbb_test
