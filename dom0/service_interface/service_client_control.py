#!/usr/bin/python
import Pyro.core, Pyro.naming
#import Service_Objects

from Pyro.errors import PyroError,NamingError


import serviceHash
from serviceHashFunction import myServiceHashFn
#for testing START
import random
#for testing END

thisMachinesIPaddress = "192.168.0.205"
LOCAL_HASH_TABLE_MAX_SIZE = 100 #maximum number of hash table entries is (LOCAL_HASH_TABLE_MAX_SIZE)
LocalHashTableList = [] #should probably be turned into a struct or something

###### PageClass Pyro object

#class PageClass(Pyro.core.ObjBase, Service_Objects.PageClass):
#		pass
class ClientSideHashTableClass(Pyro.core.SynchronizedObjBase):
	LOCAL_HASH_TABLE_MAX_SIZE = 3 #maximum number of hash table entries is (LOCAL_HASH_TABLE_MAX_SIZE)
	LocalHashTableList = [] #should probably be turned into a struct or something

	def dummyy(s):
		return "hello andi the client side thing works"
	def receivedForScanning(s, scanThisObj) :
		#scanThisObj arrives as: [pageList,processID, processAge, pageListDomainID]
		print "it actually works now"
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
			return "b"
		else :
			return "g"

	#Functions to be called inside the service interface => This is the service interface 
	#private functions, only accessible to the interface itself, NOT to the client (Chris) directly
	#########################################################################################
	#returns "g" if the data is ok to execute, and "b" if the data is bad => stop execution immediately
	#determines if the specified object already exists in the list by comparing its attributes
	# returns 1 if hashClassObj already is in list
	def searchHashList(self, hashClassObj) :
		listName = LocalHashTableList
		i = len(listName) - 1
		while i >= 0 :
			if hashClassObj.hashKey == listName[i].hashKey: 
				if hashClassObj.id1  == listName[i].id1  : 
					if hashClassObj.id2  == listName[i].id2:
						print listName[i].status
						return ([1,listName[i].status])
			i = i - 1
		return [0,"value not used if element at index zero is 0"]





	#once the page table grows too large it will be cleaned up
	#returns the cleaned up list
	def cleanup(self, thisList) :
		#print "we are cleaning", len(thisList)
		#time.sleep(5)
		n = 0
		#implements FIFO
		#while n < len(thisList)  :
		if len(thisList) > 0 :
			thisList.remove(thisList[0])
	
	def masterCleanUp(self) :
		if len(LocalHashTableList) >= LOCAL_HASH_TABLE_MAX_SIZE :
			print "we are cleaning the LOCAL HASH TABLE"
			self.cleanup(LocalHashTableList)

	def appendList(self, hashClassObj) :
		## before appending, the list is checked again to see if other process hasn't added anything yet
		searchResultss = self.searchHashList(hashClassObj)
		if  searchResultss[0] == 1 : #page already exists intable
			return

		else : #page does NOT exists => entire process is new => need to ship everything off
				LocalHashTableList.append(hashClassObj)
		print""
		print ("LOCAL Hash Table Load Factor : " , (len(LocalHashTableList)/ 1.0 / LOCAL_HASH_TABLE_MAX_SIZE ))
		print "we have ", len(LocalHashTableList) ," enrtries in the LOCAL HASH TABLE"
		print ""

	

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
 					# 'clientHashTableInterfaceObj' is the name by which our object will be known to the outside world
					ns.unregister('clientHashTableInterfaceObj')
			except NamingError:
					pass
	

			daemon.connect(ClientSideHashTableClass(),'clientHashTableInterfaceObj')
			
	
    	    # enter the server loop.
	#		print 'Server object "remotePageObject" ready.'
			print 'Server object "clientHashTableInterfaceObj" ready.'
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
				URI=ns.resolve('clientHashTableInterfaceObj')
				print 'URI:',URI
			except NamingError,x:
				print 'Couldn\'t find object, nameserver says:',x
				raise SystemExit
		
			# create a proxy for the Pyro object, and return that
			clientHashTableInterfaceObj = Pyro.core.getProxyForURI(URI)

if __name__=="__main__":
		main()


