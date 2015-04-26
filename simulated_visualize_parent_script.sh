#!/bin/bash

octave ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/matlab_files/generateinfinitewire.m

cd ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/aruco/build-aru_3_board_stuff-Desktop-Default/
./based_on_aruco_test    ~   ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/input_data_files/aruco_input_parameters/params_for_visualizing.yml  

