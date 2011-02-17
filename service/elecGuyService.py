#!/usr/bin/python
import Pyro.core, Pyro.naming
import Service_Objects

from Pyro.errors import PyroError,NamingError

#for testing
import random

###### PageClass Pyro object


thisMachinesIPaddress = "192.168.0.205"
#class PageClass(Pyro.core.ObjBase, Service_Objects.PageClass):
#		pass
class scannerInterfaceClass(Pyro.core.SynchronizedObjBase):
		def dummyy(s):
			return "hello andi"
		def receivedForScanning(s, scanThisObj) :
			#scanThisObj arrives as: [pageList,processID, processAge, pageListDomainID]
			print "elec guy is doing work now"
	
			n = 0
			sumP = 0
			while n < len(scanThisObj[0]) :
				print "page ", n
			#	print scanThisObj[0][n] #prints the contents of the page
				nn = 0
				
				while nn <= len(scanThisObj[0][n]): #should equal to 4096 
					try:
						sumP += int(scanThisObj[0][n][nn])
						nn += 1
					except :
						sumP += 1
						nn += 1
				n += 1
				print "sumP % 3 = " ,(sumP % 3)
			sumP = random.randrange(1,3)
			if sumP == 1 :
				print 'returning "b"'
				return "b"
			else :
				print 'returning "g"'
				return "g"

###### main server program

def main():
	#while True :  #do not activate this. It works properly just as often as it does not
	#	try:
			acceptedPassPhrases = ["testP"]
			Pyro.config.PUBLIC_HOST = thisMachinesIPaddress   #NOTE: this should not be hardcoded in final version
			#Pyro.config.PUBLIC_HOST = "127.0.0.1"			
			Pyro.core.initServer()
			daemon = Pyro.core.Daemon(host=thisMachinesIPaddress)
		
			# locate the NS
			locator = Pyro.naming.NameServerLocator()
			print 'searching for Name Server...'
			ns = locator.getNS()
			daemon.useNameServer(ns)
    	    # connect a new object implementation (first unregister previous one)
			try:
 					# 'scannerInterfaceObj' is the name by which our object will be known to the outside world
					ns.unregister('scannerInterfaceObj')
			except NamingError:
					pass
	

			daemon.connect(scannerInterfaceClass(),'scannerInterfaceObj')
			
	
    	    # enter the server loop.
	#		print 'Server object "remotePageObject" ready.'
			print 'Server object "scannerInterfaceObj" ready.'
			daemon.requestLoop()
	#	except :
	#		print "server initialisation didn't work"


			# connects to the scanner service ... hopefully
			locator = Pyro.naming.NameServerLocator()
			print 'Searching Name Server...',
			ns = locator.getNS()
			# resolve the Pyro object
			print 'finding object'
			try:
				URI=ns.resolve('scannerInterfaceObj')
				print 'URI:',URI
			except NamingError,x:
				print 'Couldn\'t find object, nameserver says:',x
				raise SystemExit
		
			# create a proxy for the Pyro object, and return that
			scannerInterfaceObj = Pyro.core.getProxyForURI(URI)

if __name__=="__main__":
		main()


