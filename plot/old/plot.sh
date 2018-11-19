#!/bin/bash

# Greg Ziegan (grz5)
# Matt Prosser (mep99)
# File to read and spawn plotting process from data transmitted over serial bus

rm plot.dat
touch plot.dat

if [[ $# -ne 1 ]]; then
  echo "Please provide port in the form: /dev/ttyACM*"
  exit 1
fi

port=$1
stty -F $port cs7 cstopb -ixon raw speed 115200

# the following regex will search for the first 6 hexadecimal values on a line and capture each
#regex="0x([[:xdigit:]]+).+0x([[:xdigit:]]+).+0x([[:xdigit:]]+).+0x([[:xdigit:]]+).+0x([[:xdigit:]]+).+0x([[:xdigit:]]+)"
regex="^([0-9]*):([0-9]*):([0-9.]*):([0-9.]*)"

#start_time=`echo $(date +%s%N) | cut -b1-13`  # conversion from seconds from epoch > milliseconds
#start_time=`echo $(($(date +%s%6N)/1000))`  # conversion from seconds from epoch > milliseconds

firstLine=true
start_time=0

read_data() {
  while read dataline; do
    echo $dataline
    if [[ $dataline =~ $regex ]]; then

      if [[ "firstLine" = true ]]; then
        start_time=${BASH_REMATCH[2]}
        firstLine=false
      fi

      #cur_time=`echo $(date +%s%N) | cut -b1-13`
      cur_time=${BASH_REMATCH[2]}
      t=$((cur_time-start_time))  # time axis range
      echo -e $t"\t"${BASH_REMATCH[1]}"\t"${BASH_REMATCH[2]}"\t"${BASH_REMATCH[3]}"\t"${BASH_REMATCH[4]} >> plot.dat  # append to data file
    fi
  done < $port
}

ctrl_c() {
  echo -en "\nExiting cleanly!\n"
  cleanup
  exit
}

cleanup() {
  kill $read_data_pid
  kill $plot_data_pid
  return $?
}

read_data & disown  # starts the reading process in background
read_data_pid=$!    # keep track of pid to exit cleanly
sleep 1
gnuplot liveplot.gnu > /dev/null & disown  # same concept as above
plot_data_pid=$!

# trap keyboard interrupt (ctrl-c)
trap ctrl_c SIGINT

while true; do read x; done  # master loop, will exit all processes upon exit
