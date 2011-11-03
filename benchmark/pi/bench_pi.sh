#!/bin/sh
start=`cat /proc/stat | grep softir | cut -f2 -d" "`
./pi_css5 1000000
end=`cat /proc/stat | grep softirq | cut -f2 -d" "`
elapsed=$((end-start))
echo $elapsed
