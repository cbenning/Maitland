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

#PID=`ps xa | grep "grep" | awk '{print $1;}'`
#PID=$(($PID+5))
#ps xa | grep "less" | awk '{print $1;}'
#ps xa | grep "less_p" | awk '{print $1;}'
#PID=$($PID+5)


python main.py 1234 watch
./sample_proc &

#./sample_proc &
#PID=`ps -e | grep "sample_proc$" | awk '{print $1;}'`
#echo $PID
#python main.py $PID watch
