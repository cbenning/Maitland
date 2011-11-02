#!/bin/sh
start=`cat /proc/stat | grep softir | cut -f2 -d" "`
gzip_upx 100MB.dat
gzip_upx -d 100MB.dat.gz
end=`cat /proc/stat | grep softirq | cut -f2 -d" "`
elapsed=$((end-start))
echo $elapsed
