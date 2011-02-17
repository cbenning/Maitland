#!/bin/sh
if [ -f /dev/process_stop ]
then
	rmmod process_stop
fi
# remove stale nodes
#rm -f /dev/malpage

insmod process_stop.ko
major=$(awk '/process_stop/ {print $1}' /proc/devices | head -1)
mknod /dev/process_stop c $major 0
chmod u+x /dev/process_stop
