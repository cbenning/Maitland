#!/bin/sh
# Maitland: A prototype paravirtualization-based packed malware detection system for Xen virtual machines
# Copyright (C) 2011 Christopher A. Benninger

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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


