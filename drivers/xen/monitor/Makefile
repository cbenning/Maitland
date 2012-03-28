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

#DEV_LIBS=/lib/modules/2.6.31.10-xen-dev/build
#DEV_LIBS=/home/chris/workspace/dev_lib/modules/2.6.31.10-xen-dev/build
#DEV_LIBS=/home/chris/workspace/dev_lib/modules/2.6.32.27/source
#DEV_LIBS=/home/chris/workspace/dev_lib/modules/linux-2.6.32-xen
#DEV_LIBS=/home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git
DEV_LIBS=/home/chris/workspace/dev_lib/modules/build-linux-2.6-pvops_x86_64

obj-m := monitor.o

all:
	make -C $(DEV_LIBS) M=$(PWD) modules

clean:
	make -C $(DEV_LIBS) M=$(PWD) clean
