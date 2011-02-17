#!/bin/sh

##copy malpage module in
cd /home/andi//malware/drivers/xen/process_stop/userSpaceTestCode/
gcc -o someC someC.c
gcc -o stopCaller stopCaller.c

cd /home/andi
rm -r linux-2.6.31.10-xen-dev/drivers/xen/process_stop
cp -rfv /home/andi/malware/drivers/xen/process_stop linux-2.6.31.10-xen-dev/drivers/xen/
cp -rfv /home/andi/malware/scripts/process_stop_script/process_stop_load.sh /home/andi/linux-2.6.31.10-xen-dev/drivers/xen/process_stop/
cp -rfv /home/andi/malware/scripts/process_stop_script/process_stop_remove.sh /home/andi/linux-2.6.31.10-xen-dev/drivers/xen/process_stop/
rm -r /home/andi/linux-2.6.31.10-xen-dev/drivers/xen/process_stop/userSpaceTestCode/


##copy malpage module in
##cd /root
##rm -r linux-2.6.31.10-xen-dev/drivers/xen/monitor
##cp -rf workspace/malware/drivers/xen/monitor linux-2.6.31.10-xen-dev/drivers/xen/

##Build new kernel
cd /home/andi/linux-2.6.31.10-xen-dev
nice -3 make -j40 CC="icecc"


