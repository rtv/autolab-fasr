#!/bin/bash

#ROBOTS=(2 3 4 5 6)
ROBOTS=(2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18)
N=17

logSuffix=$1

for (( g=0; g<$N; g++))
do
  for ((b=0; b<$N; b++))
  do
    green=${ROBOTS[${g}]}
    blue=${ROBOTS[${b}]}
    echo running with green: ${green} blue: ${blue}
    echo "TASK: green =" ${green}  > ratio.dat
    echo "TASK: blue =" ${blue}  >> ratio.dat
    
    stage -g -c world/fasr_fixed.world
    mv fasr.log data/bruteforce2_${logSuffix}_${green}_${blue}.log
    mv fasr_analyst.log data/bruteforce_analyst_${logSuffix}_${green}_${blue}.log
  done
done