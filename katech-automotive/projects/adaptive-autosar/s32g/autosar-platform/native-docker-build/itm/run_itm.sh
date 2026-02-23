#!/bin/bash
unalias -a
set -Euo pipefail
#trap 's=$?; echo "$0: Error $s on line "$LINENO": $BASH_COMMAND"; exit $s' ERR
IFS=$'\n\t'
shopt -s failglob

trap ctrl_c INT

ctrl_c () {
	echo "Stopping ITM after completing current run"
	runs=0
}

echoerr()
{
	printf "%s " "$@" >&2
	printf "\n" >&2
}

showstatus() {
	echo "====================================================="
	echo "Runs: ${run}, success rate=$((${#PASSED[@]}*100/run))"
	( IFS=","; echo "Passed: ${PASSED[*]}")
	( IFS=","; echo "Failed: ${FAILED[*]}")
	echo "====================================================="
}

usage()
{
	echoerr "Usage: $(basename "$1") [options]"
	echoerr "where [options] are:"
	echoerr "  -h  print this help dummary and exit"
	echoerr "  -r  test runs to execute (if not specified runs until cancelled)"
	echoerr "  -d  folder to store logfiles in (default \"itmlogs\")"
}

main()
{
	local directory="itmlogs"
	runs=-1
	while getopts :hd:r: opt; do
		case ${opt} in
		(h)
			usage "$0"
			exit 0
			;;
		(r)
			runs="${OPTARG}"
			;;
		(d)
			directory="${OPTARG}"
			;;
		(\?)
			echoerr "$0: Invalid option '${OPTARG}'"
			usage "$0"
			exit 1
			;;
		(:)
			echo "$0: Option '${OPTARG}' needs a parameter"
			usage "$0"
			exit 1
			;;
		esac
	done
	shift $((OPTIND - 1))

	FAILED=()
	PASSED=()
	echo "Creating ${directory}"
	mkdir -p "${directory}"
	run=0
	# use the testcase definition from ara-api, but remove the tests that are not applicable to the docker environment
	jq --indent 4 \
		" .\"FREERUN_PATTERN\".\"start-up\" |= . - [\"Linux version\", \
													\"-yocto-standard\"] \
		| .\"FREERUN_PATTERN\".\"init\" |= . - [\"/run successfully mounted\", \
												\"/tmp successfully mounted\", \
												\"Network initialization script terminated successfully.\", \
												\"Forked child: /opt/ara_getty/bin/ara_getty\", \
												\"Forked child: /opt/ara_dropbear/bin/ara_dropbear\"] \
		| del(.FREERUN_PATTERN.DIAG) \
		| .\"FREERUN_PATTERN\".\"Integration tests\" |= . - [\"INTEGRATION TEST <emo_integration_test> {SUCCESS}\"] \
		| .\"FREERUN_PATTERN\".\"Integration tests\" |= . + [\"INTEGRATION TEST <TC_VPKGMGR_OTAClient> {SUCCESS}\"]" \
		../../ara-api/tools/integration/itm/itm_freerun_testcases.json \
		> itm_freerun_testcases.json
	sed -i -e "s:/opt/:/tmp/apd/opt/:g" -e "s:/usr/:/tmp/apd/usr/:g" -e "s:/var/:/tmp/apd/var/:g" itm_freerun_testcases.json
	while [ $run -lt $runs ] || [ $runs -lt 0 ]; do
		echo "Starting run ${run}"
		if make itm LOGDIR=${directory} RUN=${run} -C ../; then
			PASSED+=($run)
		else
			FAILED+=($run)
		fi
		run=$((run+1))
		showstatus
	done
}

main "$@"
