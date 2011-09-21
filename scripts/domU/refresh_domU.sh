#!/bin/sh

PROJ_ROOT=/home/chris/workspace/malware_trunk

scp chris@cloudy.cs.uvic.ca:"\
$PROJ_ROOT/scripts/domU/load_domU.sh \
$PROJ_ROOT/scripts/domU/refresh_domU.sh \
$PROJ_ROOT/scripts/domU/run_watch.sh \
$PROJ_ROOT/drivers/xen/malpage/malpage.ko \
$PROJ_ROOT/domU/c_target_sample/sample_proc \
$PROJ_ROOT/domU/c_target_sample/loader \
$PROJ_ROOT/domU/python_client/main.py" .
sh load_domU.sh
