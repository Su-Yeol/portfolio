#!/bin/bash

directory=$1
run=$2

echo "Starting run $run in $directory"

waitthenkill()
{
	processname=$1
	waitctr=$2
	while pgrep -f $processname > /dev/null; do
		if [ $waitctr -gt 0 ]; then
			echo "Waiting for $processname"
			sleep 5
			waitctr=$((waitctr-1))
		else
			echo "Killing $processname"
			sudo pkill --signal 9 -f $processname > /dev/null
			sleep 1
		fi
	done
}

testphase()
{
	echo "===================================="
	echo "Testphase $1"
	echo "===================================="
	if [ $# -eq 3 ]; then
		/tmp/apd/sbin/smtool setstate $3
	elif [ $# -eq 4 ]; then
		/tmp/apd/sbin/smtool setstate -g $3 $4
	fi
	for i in $(seq -w $2 -1 0); do 
		echo -ne "$i"'\r';
		sleep 1;
	done;
}

# build and install
echo "Building and installing"
./docker_build.sh

mkdir /tmp/itmlogs
cp /mnt/native-docker-build/dlt_logstorage.conf /tmp/itmlogs

cat <<EOF | sudo tee -a /etc/dlt.conf >/dev/null
OfflineLogstorageMaxDevices = 1
OfflineLogstorageDirPath = /tmp/itmlogs
OfflineLogstorageTimestamp = 0
OfflineLogstorageDelimiter = _
EOF

# run execution manager
/tmp/apd/sbin/execution-manager ARA_ROOT=/tmp/apd/ &

testphase Driving 60
testphase Parking 10 Parking
testphase "UCM ITM test" 90 /apd/Machines/RadarFusionMachine/FunctionGroupSets/RadarFusionFunctionGroups/UCMDemoModeFG UCM_ITM
testphase "VUCM ITM test" 90 /apd/Machines/RadarFusionMachine/FunctionGroupSets/RadarFusionFunctionGroups/UCMDemoModeFG VUCM_ITM

# run ITM test parser and copy the file to logdir (only if failed)
python3 /mnt/native-docker-build/itm/itm_test.py > /tmp/itmlogs/_itm.json
ITM_PASS=$?
if [ ${ITM_PASS} -ne 0 ]; then
    echo "ITM result run ${run} failed"
    grep "Failed" /tmp/itmlogs/_itm.json
    cp /tmp/itmlogs/_itm.json /mnt/native-docker-build/itm/${directory}/itm_${run}.json
else
    echo "ITM result run ${run} passed"
fi
# archive logfiles for debug inspection
echo "Archiving logfiles"
cp /var/redirected/* /tmp/itmlogs
tar cJf /tmp/logfile_$run.tar.xz --warning=no-file-changed -C /tmp/itmlogs .

echo "Copying to /mnt/native-docker-build/itm/${directory}/logfile_$run.tar.xz"
cp /tmp/logfile_$run.tar.xz /mnt/native-docker-build/itm/${directory}
echo "Run $run completed"
exit ${ITM_PASS}
