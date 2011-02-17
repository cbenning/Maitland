#!/usr/bin/python

import fcntl, os, sys
from array import array

#Driver
GENSHMF_XS_REGISTER_PATH = "/genshm/register"
GENSHMF_DEVICE = "/dev/genshm"


#Commands
GENSHMF_IOC_MAGIC = 250
GENSHMF_REPORT = GENSHMF_IOC_MAGIC+1
GENSHMF_PAGEINFO = GENSHMF_IOC_MAGIC+2
GENSHMF_PFNLIST = GENSHMF_IOC_MAGIC+3
GENSHMF_PFNCLR = GENSHMF_IOC_MAGIC+4
GENSHMF_PFNCOUNT = GENSHMF_IOC_MAGIC+5
GENSHMF_PFNLISTSHOW = GENSHMF_IOC_MAGIC+6
GENSHMF_REGISTER = GENSHMF_IOC_MAGIC+8
GENSHMF_TEST = GENSHMF_IOC_MAGIC+9

class GENSHMF():
	def __init__(self,fileName):
		self._filehandle = file(fileName,'r')
		self._output = array('I')
		
	def close(self):
		self._filehandle.close()
	
	def doGENSHMFOp(self, cmd, pid):
		return fcntl.ioctl(self._filehandle, cmd, pid)



def main():

	if(len(sys.argv)!=3):
		print("not enough params")
		exit(1)
	
	pid = int(sys.argv[1])
	ops = GENSHMF(GENSHMF_DEVICE)
	
	if(sys.argv[2] == "register"):
		ops.doGENSHMFOp(GENSHMF_REGISTER, pid)
	
	elif(sys.argv[2] == "report"):
		#Report
		ops.doGENSHMFOp(GENSHMF_REPORT, pid)		

	elif(sys.argv[2] == "test"):
		#Test
		ops.doGENSHMFOp(GENSHMF_TEST, pid)		
	

	else:
		print "Command unknown"

	ops.close()

main()
