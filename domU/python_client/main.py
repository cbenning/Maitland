#!/usr/bin/python
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

import fcntl, os, sys
from array import array

#Driver
MALPAGE_DEVICE_NAME="malpage"
MALPAGE_XS_REGISTER_PATH = "/malpage/register"
MALPAGE_XS_REPORT_PATH = "/malpage/report"
MALPAGE_DEVICE = "/dev/"+MALPAGE_DEVICE_NAME


#Commands
MALPAGE_IOC_MAGIC = 250
MALPAGE_REPORT = MALPAGE_IOC_MAGIC+1
MALPAGE_PAGEINFO = MALPAGE_IOC_MAGIC+2
MALPAGE_PFNLIST = MALPAGE_IOC_MAGIC+3
MALPAGE_PFNCLR = MALPAGE_IOC_MAGIC+4
MALPAGE_PFNCOUNT = MALPAGE_IOC_MAGIC+5
MALPAGE_PFNLISTSHOW = MALPAGE_IOC_MAGIC+6
MALPAGE_REGISTER = MALPAGE_IOC_MAGIC+8
MALPAGE_TEST = MALPAGE_IOC_MAGIC+9
MALPAGE_WATCH = MALPAGE_IOC_MAGIC+10


class Malpage():
	def __init__(self,fileName):
		self._filehandle = file(fileName,'r')
		self._output = array('I')
		
	def close(self):
		self._filehandle.close()
	
	def doMalpageOp(self, cmd, pid):
		return fcntl.ioctl(self._filehandle, cmd, pid)


def main():

	if(len(sys.argv)!=3):
		print("not enough params")
		exit(1)
	
	pid = int(sys.argv[1])
	ops = Malpage(MALPAGE_DEVICE)
	
	if(sys.argv[2] == "register"):
		ops.doMalpageOp(MALPAGE_REGISTER, pid)
	
	elif(sys.argv[2] == "report"):
		#Report
		ops.doMalpageOp(MALPAGE_REPORT, pid)		

	elif(sys.argv[2] == "test"):
		#Test
		ops.doMalpageOp(MALPAGE_TEST, pid)		
		
	elif(sys.argv[2] == "watch"):
		#Test
		ops.doMalpageOp(MALPAGE_WATCH, pid)		

	else:
		print "Command unknown"

	ops.close()

main()
