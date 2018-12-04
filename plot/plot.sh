#!/bin/bash


#stty -F /dev/ttyUSB0 9600
#stty -F $1 115200
#cat $1 | rg -M 16 "temp:   " --line-buffered | cat -n | tee data &
tail -f $1 | rg "report: " --line-buffered | cat -n > data &
tail -f $1 | rg -v "report: " --line-buffered &
gnuplot plot.gnuplot

trap 'kill $(jobs -p)' EXIT

