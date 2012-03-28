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


PROJ_ROOT=/home/chris/workspace/malware_trunk

scp chris@cloudy.cs.uvic.ca:"\
$PROJ_ROOT/scripts/domU/load_domU.sh \
$PROJ_ROOT/scripts/domU/refresh_domU.sh \
$PROJ_ROOT/scripts/domU/run_watch.sh \
$PROJ_ROOT/drivers/xen/malpage/malpage.ko \
$PROJ_ROOT/domU/c_target_sample2/runme2 \
$PROJ_ROOT/domU/c_target_sample2/runme2_upx \
$PROJ_ROOT/domU/c_target_sample2/runme2_gzexe \
$PROJ_ROOT/benchmark/pi/bench_* \
$PROJ_ROOT/benchmark/pi/pi_css5* \
$PROJ_ROOT/benchmark/gzip/bench_* \
$PROJ_ROOT/benchmark/gzip/gzip* \
$PROJ_ROOT/benchmark/gettime \
$PROJ_ROOT/benchmark/getsoftirqs.sh \
$PROJ_ROOT/domU/c_loader/loader \
$PROJ_ROOT/domU/python_client/main.py" .
#sh load_domU.sh
#$PROJ_ROOT/benchmark/gzip/10MB.dat \
