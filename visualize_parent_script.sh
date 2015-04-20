#!/bin/bash

# run octave to mesh files together based on times of reading, then write yaml
patt=~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/
octave $patt/matlab_files/new_csv_to_YAML_script.m

# run aruco to show vectors
#patt=~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/
#$patt/aruco/aru_3_board_stuff/just_run_visualize.sh
cd /home/ausername/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/aruco/aru_3_board_stuff
./just_run_visualize.sh
