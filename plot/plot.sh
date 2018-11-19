#!/bin/bash


#stty -F /dev/ttyUSB0 9600
stty -F /dev/ttyACM1 115200
cat /dev/ttyACM1 | rg -M 8 "" --line-buffered | cat -n | tee data &
sleep 5
gnuplot plot.gnuplot &

trap 'kill $(jobs -p)' EXIT

