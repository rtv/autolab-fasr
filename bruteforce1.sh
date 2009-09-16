#!/bin/bash

#ROBOTS=(2 3 4 5 6)
ROBOTS=(0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18)
N=19

# name of configuration, e.g.1:1
CONF="1:1"
TASK="task_bf_"$CONF".txt"
RATIO="ratio_bf_"$CONF".txt"

echo "TASK: 0 green sourceA sinkA 1 0.03 10.0" > ${TASK}
echo "TASK: 0 blue sourceB sinkB 1 0.03 10.0" >> ${TASK}

for (( g=0; g<$N; g++))
do
  for ((b=0; b<$N; b++))
  do
    GREEN=${ROBOTS[${g}]}
    BLUE=${ROBOTS[${b}]}

    LOG="./data/fixed_"$CONF$"_"$GREEN"_"$BLUE".dat"
    echo running with green: ${GREEN} blue: ${BLUE}
    echo "TASK: green =" ${GREEN}  > $RATIO
    echo "TASK: blue =" ${BLUE}  >> $RATIO

    stage -g -c -rf=$RATIO -td=$TASK -lf=$LOG world/fasr_fixed.world
    rm fasr.log
  done
done