#!/bin/bash

echo "########   Starting 06_01_04_A_1   ########"
echo "########   Interval = 2^-3s   ########"
cd ../../../
./daemon_cl_log pfe0 -F conf/Section_1/1_Common_AED_Tests/06_01_04_A_1.ini &
PID1=$!
sleep 20
kill $PID1

echo "#########   Starting 06_01_04_A_2.sh   ########"
echo "########   Interval = 2^-4s   ########"
./daemon_cl_log pfe0 -F conf/Section_1/1_Common_AED_Tests/06_01_04_A_2.ini &
PID2=$!
sleep 20
kill $PID2

echo "########Starting 06_01_04_A_3.sh########"
echo "########   Interval = 2^-5s   ########"
./daemon_cl_log pfe0 -F conf/Section_1/1_Common_AED_Tests/06_01_04_A_3.ini &
PID3=$!
sleep 20
kill $PID3

