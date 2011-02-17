#!/bin/sh

#/home/chris/workspace/malware/drivers/xen/monitor/monitor.ko \

scp chris@cloudy.cs.uvic.ca:"\
/home/chris/workspace/malware/scripts/dom0/load_dom0.sh \
/home/chris/workspace/malware/scripts/dom0/refresh_dom0.sh \
/home/chris/workspace/malware/drivers/xen/genshm-back/genshm-back.ko \
/home/chris/workspace/malware/dom0/genshmb/main.py" .
#sh load_dom0.sh
