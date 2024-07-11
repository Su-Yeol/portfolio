#!/bin/bash

./06_01_03_A_1.sh
PID1=$!
sleep 30
kill $PID1

./06_01_03_A_2.sh 
PID2=$!
sleep 30
kill $PID2

./06_01_03_A_3.sh.sh 

