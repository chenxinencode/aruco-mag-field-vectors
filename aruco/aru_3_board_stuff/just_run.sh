#!/bin/bash

#path to this directory
patt1=~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/aruco/aru_3_board_stuff/build

$patt1/based_on_aruco_test live   $patt1/td/22board2config.yml   $patt1/td/22board3config.yml    $patt1/td/lab_frame_board_config.yml     $patt1/td/intrinsics.yml 0.025 0.033 0 -1 -0.5 0 0 1
