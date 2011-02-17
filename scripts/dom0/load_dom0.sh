#!/bin/sh
#if [ -f /dev/monitor ]
#then
rmmod genshm-back.ko
#modprobe -r monitor
#fi
# remove stale nodes
#rm -f /dev/monitor

insmod genshm-back.ko
#major=$(awk '/monitor/ {print $1}' /proc/devices | head -1)
#mknod /dev/monitor c $major 0
