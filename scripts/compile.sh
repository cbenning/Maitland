#!/bin/sh

##copy malpage module in
cd /root/workspace/malware/drivers/xen/malpage
#make clean
make 
#cd /root
rm -r /root/linux-2.6.31.10-xen-dev/drivers/xen/malpage
cp -rf /root/workspace/malware/drivers/xen/malpage /root/linux-2.6.31.10-xen-dev/drivers/xen/

##copy monitor module in
cd /root/workspace/malware/drivers/xen/monitor
#make clean
make
#cd /root
rm -r /root/linux-2.6.31.10-xen-dev/drivers/xen/monitor
cp -rf /root/workspace/malware/drivers/xen/monitor /root/linux-2.6.31.10-xen-dev/drivers/xen/

##Build clients
#cd /root/workspace/malware/domU/c_client
#make

##Build clients
#cd /root/workspace/malware/dom0/c_client
#make

##Build new kernel
#cd /root/linux-2.6.31.10-xen-dev
#make -j40 CC="icecc" KBUILD_VERBOSE=1
#make -j40 CC="icecc" install
#make -j40 CC="icecc" modules_install

##Setup image
#cd /boot
#depmod -a 2.6.31.10-xen-dev
#mkinitramfs -o initrd.img-2.6.31.10-xen-dev 2.6.31.10-xen-dev


