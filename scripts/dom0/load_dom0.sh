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

#if [ -f /dev/monitor ]
#then

rmmod monitor.ko

#modprobe -r monitor
#fi
# remove stale nodes
#rm -f /dev/monitor

insmod monitor.ko

#major=$(awk '/monitor/ {print $1}' /proc/devices | head -1)
#mknod /dev/monitor c $major 0
