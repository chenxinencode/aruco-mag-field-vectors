#!/bin/bash

patt=~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/

#run python to do convert from scatter
python $patt/python_files/csv_to_YAML_newer_than_octave.py

# run octave. This just writes yaml
octave $patt/matlab_files/just_run_quiver3AR_wrapper.m

# run aruco to show vectors
#patt=~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/
#$patt/aruco/aru_3_board_stuff/just_run_visualize.sh



#most recent
#cd ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/aruco/aru_3_board_stuff
#./just_run_visualize.sh


cd ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/aruco/build-aru_3_board_stuff-Desktop-Default/
./based_on_aruco_test    ~   ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/input_data_files/aruco_input_parameters/params_for_visualizing.yml  

