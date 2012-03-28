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


start=`cat /proc/stat | grep softir | cut -f2 -d" "`
stime=`./gettime`


./gzip 100MB.dat
./gzip -d 100MB.dat.gz


end=`cat /proc/stat | grep softirq | cut -f2 -d" "`
etime=`./gettime`

starr=(${stime// / })
enarr=(${etime// / })
irqs=$((end-start))


stsec=${starr[0]}
stusec=${starr[1]}
ensec=${enarr[0]}
enusec=${enarr[1]}

#echo $stsec
#echo $stusec
#echo $ensec
#echo $enusec

esec=$((ensec-stsec))
eusec=$((enusec-stusec))
if [ "$eusec" -lt 0 ]
    then
    esec=$((esec-1))
    eusec=$((1000000+eusec))
fi

eusec=`printf "%06d" $eusec`
echo $irqs
echo "${esec}.${eusec}"
#echo $eusec





