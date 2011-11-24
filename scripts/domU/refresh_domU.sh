#!/bin/sh

PROJ_ROOT=/home/chris/workspace/malware_trunk

scp chris@cloudy.cs.uvic.ca:"\
$PROJ_ROOT/scripts/domU/load_domU.sh \
$PROJ_ROOT/scripts/domU/refresh_domU.sh \
$PROJ_ROOT/scripts/domU/run_watch.sh \
$PROJ_ROOT/drivers/xen/malpage/malpage.ko \
$PROJ_ROOT/domU/c_target_sample2/runme2 \
$PROJ_ROOT/domU/c_target_sample2/runme2_upx \
$PROJ_ROOT/domU/c_target_sample2/runme2_gzexe \
$PROJ_ROOT/benchmark/pi/bench_* \
$PROJ_ROOT/benchmark/pi/pi_css5* \
$PROJ_ROOT/benchmark/gzip/bench_* \
$PROJ_ROOT/benchmark/gzip/gzip* \
$PROJ_ROOT/benchmark/gettime \
$PROJ_ROOT/benchmark/getsoftirqs.sh \
$PROJ_ROOT/domU/c_loader/loader \
$PROJ_ROOT/domU/python_client/main.py" .
#sh load_domU.sh
#$PROJ_ROOT/benchmark/gzip/10MB.dat \
