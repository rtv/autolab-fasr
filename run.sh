#!/bin/bash

# clear all log files
rm *.log

#############################################################
# first configuration 1:1 (0.03 0.03)
echo "TASK: 0 green sourceA sinkA 1 0.03 10.0" > tasks.txt
echo "TASK: 0 blue sourceB sinkB 1 0.03 10.0" >> tasks.txt

./run_experiment1.sh "1:1"
./bruteforce.sh "1:1"

#############################################################
# first configuration 2:1 (0.04 0.02)
echo "TASK: 0 green sourceA sinkA 1 0.04 10.0" > tasks.txt
echo "TASK: 0 blue sourceB sinkB 1 0.02 10.0" >> tasks.txt

./run_experiment1.sh "2:1"
./bruteforce.sh "2:1"

#############################################################
# first configuration 10:1 (0.03 0.03)
echo "TASK: 0 green sourceA sinkA 1 0.0545 10.0" > tasks.txt
echo "TASK: 0 blue sourceB sinkB 1 0.0055 10.0" >> tasks.txt

./run_experiment1.sh "10:1"
./bruteforce.sh "10:1"