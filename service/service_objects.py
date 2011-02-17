#!/usr/bin/python
# Services_Objects.py


#this one actually does the real work of servive_server.py


import service_hash
import Pyro.core
import random

#for testing
import time





REPEATS_IF_CONN_DENIED = 10  #number of attempts to establish connection to scanning service
PAGE_TABLE_MAX_SIZE = 16 #maximum number of page table entries is (PAGE_TABLE_MAX_SIZE / 2) => obsolete 
HASH_TABLE_MAX_SIZE = 100 #maximum number of hash table entries is (HASH_TABLE_MAX_SIZE)
PageTableList = [] # => obsolote
HashTableList = [] #should probably be turned into a struct or something


#these  functions are local only
#####################################################################################
#####################################################################################

#determines if the specified object already exists in the list  "=="
#returns 1 if "searchFor" already exists in the list
def searchPageList(listName, searchFor):
	if searchFor in listName:
		return 1


	
#determines if the specified object already exists in the list by comparing its attributes
# returns 1 if hashClassObj already is in list

#*********This will be affected if the data changes*********#
def searchHashList(listName, hashClassObj) :
	i = len(listName) - 1
	while i >= 0 :
		if hashClassObj.hashKey == listName[i].hashKey: 
			if hashClassObj.id1  == listName[i].id1  : 
				if hashClassObj.id2  == listName[i].id2:
					print listName[i].status
					listName.append(listName[i]) #needed, to implement stack based LRU implementation
					listName.remove(listName[i]) #needed, to implement stack based LRU implementation
					return ([1,listName[len(listName)-1].status])
		i = i - 1
	return [0,"value not used if element at index zero is 0"]



#once the page table grows too large it will be cleaned up
#returns the cleaned up list
def cleanup(thisList) :
	print "we are cleaning", len(thisList)
	#time.sleep(5)

#THIS IMPLEMENTS LRU BECAUSE OF HOW THE LIST IS HANDLED IN searchHashList()
#it assumes a stack based LRU implementation (note: also works for FIFO)
	if len(thisList) > 0 :
		thisList.remove(thisList[0])
	


#displays the contents of any list
def display(listName):
	print "####################################################"
	n = 0
	while n < len(listName) :
		print listName[n]
		n = n + 1


#sends the data off to "elec-guy" (Niko ???)
def send_page_off_for_scanning(scanThisObj) :

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
		raise SystemExit #this is bad, very bad
		# perhaps use this
		#time.sleep(5)
		#sendOffForScanning(scanThisObj) #careful of halting problem

	# create a proxy for the Pyro object, and return that
	scannerInterfaceObj = Pyro.core.getProxyForURI(URI)
	
	#print scannerInterfaceObj.dummyy() #prints "hello andi"
	print "it should go to scanning right here"
	repeats = REPEATS_IF_CONN_DENIED
	sleepT = time.sleep(random.randrange(5, 10))	
	while repeats >= 0 : # scanner might be maxed out on connections
		repeats -= 1
		try :
			return scannerInterfaceObj.receivedForScanning(scanThisObj) #elecScan(scanThisObj)
		except ConnectionDeniedError, x:
			time.sleep(int(sleepT)*int(sleepT)) #using exponential backoff for trying to connect upwards in the network
			return scannerInterfaceObj.receivedForScanning(scanThisObj) #elecScan(scanThisObj)



# These classes will be remotely accessed.
######################################################################################
######################################################################################
class PageClass: #obsolete now
	
	#Start::older definitions for testing purposes only
	def mul(s, arg1, arg2): return arg1*arg2
	def add(s, arg1, arg2): return arg1+arg2
	def sub(s, arg1, arg2): return arg1-arg2
	def div(s, arg1, arg2): return arg1/arg2
	#End::older definitions for testing purposes only

	def  PageTableLoadFactor(s) :
		return ("Page Table Load Factor : " , (len(PageTableList) / 1.0 / PAGE_TABLE_MAX_SIZE ))




	#this is the actual function to send off pages
	def submitNewPage(s, pageObj, hashClassObj):
		#send page off and record results
		hashClassObj.status = send_page_off_for_scanning(pageObj)
		# write the result of this into the hashTable
		HashTableList.append(hashClassObj)
		#return the result to caller
		return hashClassObj.status


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
class HashClassInService:
	
	#Start:older definitions for testing purposes only
	def show(s, arg1, arg2): return arg1*arg1
	#End::older definitions for testing purposes only



	def  HashTableLoadFactor(self, s) :
		return ("Hash Table Load Factor : " , (len(HashTableList)/ 1.0 / HASH_TABLE_MAX_SIZE ))



	def submitHash(s, processObj):
		display(HashTableList) #for testing, shows what is in the list currently
	#*********This comment will be affected if the data changes*********#
	#	processObj looks like this:  [pageList,processID, processAge, pageListDomainID] 
		print "\narrived"
		print "this is the length of the list ", len(HashTableList)
		#find pages in table
		n = len(processObj[0]) - 1
		while n >= 0 :
			#find each page from the pageList and compare to entries in hashTable
			#*********This will be affected if the data changes*********#
			hashClassObj = serviceHash.HashClass(processObj[0][n], processObj[1], processObj[2], processObj[3] )
			searchResults = searchHashList(HashTableList, hashClassObj)
			print "search result: ", searchResults[0]
			if  searchResults[0] : #page already exists intable

				print "object exists in list already => WE HAVE A HIT, CONGRATULATIONS"
				if searchResults[1] == "b" :
					return searchResults[1]

			else : #page does NOT exists => entire process is new => need to ship everything off

				print "object DOES NOT exist in list already"
				status = send_page_off_for_scanning(processObj)
				n = len(processObj[0]) - 1	

				while n >= 0 :

					#*********This will be affected if the data changes*********#
					hashClassObj = serviceHash.HashClass(processObj[0][n], processObj[1], processObj[2], processObj[3] )
					hashClassObj.status = status
					#PageTableList is full
					if len(HashTableList) >= HASH_TABLE_MAX_SIZE :
						cleanup(HashTableList)
					HashTableList.append(hashClassObj)
					n = n - 1
				print status , "is the status"
				if status == "b" :
					return status
				
			
			n = n - 1
			
		return "g"		

	


