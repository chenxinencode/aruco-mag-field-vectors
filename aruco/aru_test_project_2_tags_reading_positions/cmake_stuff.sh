#!/bin/bash

rm -rf build
mkdir build
cd build
cmake ..
make


mkdir td

cp /home/elabbiglubu/Downloads/aru_testdata/single/intrinsics.yml ./td/

##cp /home/elabbiglubu/Downloads/aru_testdata/board/* ./td/
##./based_on_aruco_test ./td/image-test.png ./td/board_meters.yml ./td/intrinsics.yml 0.039

##cp /home/lubuntu/Desktop/aruco-1.2.4/build/utils/testdata/single/* ./td/
##./aruco_simple ./td/image-test.png ./td/intrinsics.yml 0.01
