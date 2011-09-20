#!/bin/sh

./sample_proc &
#sleep 3

PID=`ps -e | grep "sample_proc$" | awk '{print $1;}'`

#PID=`ps xa | grep "grep" | awk '{print $1;}'`
#PID=$(($PID+5))
echo $PID
python main.py $PID watch
#./less main.py &
#./less_p main.py &
#ps xa | grep "less" | awk '{print $1;}'
#ps xa | grep "less_p" | awk '{print $1;}'
#PID=$($PID+5)
#echo $PID
