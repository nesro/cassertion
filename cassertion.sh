#!/bin/bash

#function usage() {
#	echo "cassertion.sh -c config_file -f log_file -l log_dir -m make -t test_dir"
#}

function run_test() {
	binary="$1"
	eof="$2"
	logpath="$3"
	
	if [[ ! -f ./$binary ]]; then
		echo "cassert error: $binary is not a file"
	fi
	
	if [[ ! -x ./$binary ]]; then
		echo "cassert error: $binary is not executable"
	fi
	
	echo $logpath
	./$binary $eof >> $logpath 2>&1
}

# --------------------------------------------------------------------------

while getopts :c:f:l:m:t: opt; do
	case $opt in
		c) config="$OPTARG" ;;
		f) logfile="$OPTARG" ;;
		l) logdir="$OPTARG" ;;
		m) make_cmd="$OPTARG" ;;
		t) testdir="$OPTARG" ;;
		*) usage ;;
	esac
done
   
config=${config:-"./cassertion_settings.txt"}
logfile=${logfile:-"$(date +%d.%m.%Y-%H:%M:%S.log)"}
logdir=${logdir:-"./cassertion_log/"}
logpath=$logdir$logfile
make_cmd=${make_cmd:-"make"}
testdir=${testdir:-"./"}

mkdir -p $logdir

if cd $testdir && $make_cmd clean && ! $make_cmd ; then
	echo "Build the source with $make in $testdir has failed"
	exit 1
fi

# Have we a test that will be run alone?
while IFS=':' read binary eof run_opt ; do
	if [[ "$run_opt" == "1" ]] ; then
		run_test $binary $eof $logpath
		exit 0
	fi
done < $config

# Have we a test that will be run alone?
while IFS=':' read binary eof run_opt ; do
	if [[ "$run_opt" == "2" ]]; then
		continue
	fi
	
	if [[ "$binary" == "#"* ]] ; then
		continue
	fi

	run_test $binary $eof $logpath

	if [[ "$run_opt" == "3" ]] ; then
		exit 0
	fi
done < $config
