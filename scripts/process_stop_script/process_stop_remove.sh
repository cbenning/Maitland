#!/bin/sh
echo "removing the PROCESS_STOP module"
rmmod --force ./process_stop.ko
rm -f /dev/process_stop
