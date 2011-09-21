#!/bin/sh


#PID=`ps xa | grep "grep" | awk '{print $1;}'`
#PID=$(($PID+5))
#ps xa | grep "less" | awk '{print $1;}'
#ps xa | grep "less_p" | awk '{print $1;}'
#PID=$($PID+5)


python main.py 1234 watch
./sample_proc &

#./sample_proc &
#PID=`ps -e | grep "sample_proc$" | awk '{print $1;}'`
#echo $PID
#python main.py $PID watch
