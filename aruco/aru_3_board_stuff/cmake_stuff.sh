#!/bin/bash

rm -rf build
mkdir build
cd build
cmake ..
make

# cd ..
# ./file_copy_stuff.sh