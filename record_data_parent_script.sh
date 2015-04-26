#!/bin/bash

patt=~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/
rm $patt/intermediate_data_files/pos_data_from_aruco.csv
rm $patt/intermediate_data_files/vectors_to_show_in_final_step.yml
#$patt/aruco/aru_3_board_stuff/just_run_record.sh >> $patt/intermediate_data_files/pos_data_from_aruco.csv 


cd ~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/aruco/
./most_recent_just_run_record.sh >> $patt/intermediate_data_files/pos_data_from_aruco.csv 
