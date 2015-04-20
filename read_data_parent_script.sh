#!/bin/bash

timeToTakeData=60

echo "Unfortunately, you can not run this more than one time per minute. jaime has not figured out yet how to make it record data for an arbitrary amount of time, so it records for this many seconds:"
echo $timeToTakeData
echo "ALSO..."
echo "You have to press Ctrl + C when you're done."

trap killallprocesses SIGINT

killallprocesses(){
  echo finishing...
  kill 0
}

patt=~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/
pattpy=$patt/python_get_data_from_arduino_and_arduino_sketch
# run python program to get arduino data, and run aruco to get location and rotation data
python $pattpy/python_read_arduino_data_v1.py $timeToTakeData >> $patt/intermediate_data_files/pythonOutty.csv &
$patt/aruco/aru_3_board_stuff/just_run.sh >> $patt/intermediate_data_files/pos_data_elephant.csv &
wait
# wait is a command

