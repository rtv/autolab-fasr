#!/bin/bash

N=20

# name of configuration, e.g.1:1 2:1 10:1
CONF="2:1"

TASK="task5.txt"

PROB=(0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0)

echo "TASK: 0 green sourceA sinkA 1 0.0545 10.0" > ${TASK}
echo "TASK: 0 blue sourceB sinkB 1 0.0055 10.0" >> ${TASK}


for ((j=0;j<11;j++))
do
  for ((i=1;i<=N;i++))
  do
    P=${PROB[${j}]}
    LOG="./data/wait_"${CONF}"_"${i}"_"${P}".dat"
    stage -g -c -td=$TASK -pb=$P -lf=$LOG world/fasr_wait.world
    rm fasr.log
  done
done
