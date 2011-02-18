#!/bin/sh

PROJ_ROOT=/home/chris/workspace/malware_trunk

scp chris@cloudy.cs.uvic.ca:"\
$PROJ_ROOT/scripts/dom0/load_dom0.sh \
$PROJ_ROOT/scripts/dom0/refresh_dom0.sh \
$PROJ_ROOT/drivers/xen/monitor/monitor.ko \
$PROJ_ROOT/dom0/python_server/main.py" .
sh load_dom0.sh
