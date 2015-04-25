#!/bin/bash

rm -rf build
mkdir build
cd build
cmake ..
make


./file_copy_stuff.sh