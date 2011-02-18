#!/bin/sh
#if [ -f /dev/malpage ]
#then

rmmod malpage.ko

#modprobe -r malpage
#fi
# remove stale nodes
#rm -f /dev/malpage

insmod malpage.ko

#major=$(awk '/malpage/ {print $1}' /proc/devices | head -1)
#mknod /dev/malpage c $major 0
