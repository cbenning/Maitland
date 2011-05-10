#!/bin/sh

./sample_proc &
sleep 3

PID=`ps -e | grep "sample_proc$" | awk '{print $1;}'`
echo $PID

python main.py $PID watch
