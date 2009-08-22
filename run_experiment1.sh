#!/bin/bash

N=10

log_suffix=$1

for ((i=1;i<=N;i++))
do
  stage -g world/fasr_wait.world
  mv fasr.log data/wait_${log_suffix}_${i}.log
  mv fasr_analyst.log data/wait_analyst_${log_suffix}_${i}.log
done

for ((i=1;i<=N;i++))
do
  stage -g world/fasr_replan.world
  mv fasr.log data/replan_${log_suffix}_${i}.log
  mv fasr_analyst.log data/replan_analyst_${log_suffix}_${i}.log
done