#!/bin/sh

PROJ_ROOT=/home/chris/workspace/malware_trunk

scp chris@cloudy.cs.uvic.ca:"\
$PROJ_ROOT/scripts/domU/load_domU.sh \
$PROJ_ROOT/scripts/domU/refresh_domU.sh \
$PROJ_ROOT/scripts/domU/run_watch.sh \
$PROJ_ROOT/drivers/xen/malpage/malpage.ko \
$PROJ_ROOT/domU/c_target_sample/sample_proc \
$PROJ_ROOT/domU/c_target_sample/sample_proc_p \
$PROJ_ROOT/domU/c_target_sample2/runme \
$PROJ_ROOT/domU/c_target_sample2/runme_p \
$PROJ_ROOT/domU/c_target_sample2/runme2 \
$PROJ_ROOT/domU/c_target_sample2/runme2_p \
$PROJ_ROOT/domU/c_loader/loader \
$PROJ_ROOT/domU/python_client/main.py" .
sh load_domU.sh
