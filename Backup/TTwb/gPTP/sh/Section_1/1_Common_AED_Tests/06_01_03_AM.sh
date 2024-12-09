#!/bin/bash

echo "########   Starting 06_01_03_A_1   ########"
echo "########   Interval = 2s   ########"
cd ../../../
./daemon_cl_log pfe0 -F conf/Section_1/1_Common_AED_Tests/06_01_03_A_1.ini &
PID1=$!
sleep 20
kill $PID1

echo "#########   Starting 06_01_03_A_2.sh   ########"
echo "########   Interval = 4s   ########"
./daemon_cl_log pfe0 -F conf/Section_1/1_Common_AED_Tests/06_01_03_A_2.ini &
PID2=$!
sleep 20
kill $PID2

echo "########Starting 06_01_03_A_3.sh########"
echo "########   Interval = 8s   ########"
./daemon_cl_log pfe0 -F conf/Section_1/1_Common_AED_Tests/06_01_03_A_3.ini &
PID3=$!
sleep 20
kill $PID3

