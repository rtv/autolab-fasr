#!/bin/bash

N=20

# name of configuration, e.g.1:1 2:1 10:1
CONF="3:1-1:3"
TASK="task8.txt"


echo "TASK: 0 green sourceA sinkA 1 0.045 10.0" > ${TASK}
echo "TASK: 3600 green sourceA sinkA 1 0.015 10.0" >> ${TASK}
echo "TASK: 0 blue sourceB sinkB 1 0.015 10.0"  >> ${TASK}
echo "TASK: 3600 blue sourceB sinkB 1 0.045 10.0"  >> ${TASK}

for ((i=1;i<=N;i++))
do
  LOG="./data/replan_"${CONF}"_"${i}".dat"
  stage -g -c -td=$TASK -lf=$LOG world/fasr_replan.world
  rm fasr.log
done
