#!/bin/bash

rm -rf build
mkdir build
cd build
cmake ..
make


mkdir td
cp ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/input_data_files/aru_testdata/single/intrinsics.yml ./td/

cp ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/input_data_files/22boardfiles/* ./td/
cp ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/input_data_files/lab_frame_board_files/* ./td/



##cp /home/elabbiglubu/Downloads/aru_testdata/board/* ./td/
##./based_on_aruco_test ./td/image-test.png ./td/board_meters.yml ./td/intrinsics.yml 0.039

##cp /home/lubuntu/Desktop/aruco-1.2.4/build/utils/testdata/single/* ./td/
##./aruco_simple ./td/image-test.png ./td/intrinsics.yml 0.01
