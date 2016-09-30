#!/bin/sh

copy_data_path=$1

if [ -z "$1" ]; then
  #echo $script_dir
  echo "usage : test_setup <test data directory>"

elif [ $1 = "/"  ]; then
  echo "usage : test_setup <test data directory>"

elif [ ! -e $1  ]; then
  echo "not found : $1"
  echo "usage : test_setup <test data directory>"

else
  echo "test data setup... [test data directory $copy_data_path]"

  cp -r ${copy_data_path}/* .

  rm test_data_03.txt
  ln -s test_data_01.txt test_data_03.txt
  rm test_data_04
  ln -s test_data_02 test_data_04
  rmdir test_data_05


  rm test2_data_03.txt
  rm test2_data_03b.txt
  ln -s test2_data_01.txt test2_data_03.txt
  ln -s test2_data_01.txt test2_data_03b.txt
  rm test2_data_04
  rm test2_data_04a
  ln -s test2_data_02 test2_data_04
  ln -s test2_data_02 test2_data_04a
  rmdir test2_data_05
  rmdir test2_data_05a
fi

