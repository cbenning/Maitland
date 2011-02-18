#!/bin/sh
#/home/chris/workspace/malware/drivers/xen/malpage/malpage.ko \

scp chris@cloudy.cs.uvic.ca:"\
/home/chris/workspace/malware/trunk/scripts/domU/load_domU.sh \
/home/chris/workspace/malware/trunk/scripts/domU/refresh_domU.sh \
/home/chris/workspace/malware/trunk/drivers/xen/genshm-front/genshm-front.ko \
/home/chris/workspace/malware/trunk/domU/c_target_sample/sample_proc \
/home/chris/workspace/malware/trunk/domU/genshmf/main.py" .
#sh load_domU.sh
