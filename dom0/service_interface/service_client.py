#!/usr/bin/python

import Pyro.naming, Pyro.core
from Pyro.errors import NamingError
from Pyro.errors import ConnectionDeniedError
import os.path
import serviceHash
from serviceHashFunction import myServiceHashFn

import time
#import os

#for testing
#from threading import Thread
import random
#import types


REPEATS_IF_CONN_DENIED = 10

NUMBER_OF_PROCESSES = 5  #do not set this above 10 ... 25 and 30 threads is definitely causing thrashing => BAD!!!!
NUMBER_OF_THREADS = 10 # do not set this above 20, just to be save ..... (50 is too much it seems) ... even 10 can take a LONG LONG time

thisMachinesIPaddress = "192.168.0.205"
# Pyro Stuff
#########################################################################################
# locate the NS

locator = Pyro.naming.NameServerLocator()
print 'Searching Name Server...',
ns = locator.getNS()

# resolve the Pyro object
print 'finding object'
repeat1 = 10
while repeat1 >= 0 :
	repeat1 = repeat1 - 1
	try:
		daemon = Pyro.core.Daemon(host=thisMachinesIPaddress)
		daemon.useNameServer(ns)
		URI1=ns.resolve('remoteHashObject')
		print 'URI1:',URI1
		break
	except NamingError,x:
		print 'Couldn\'t find object, nameserver says:',x
		if repeat1 <= 0:
			raise SystemExit

# create a proxy for the Pyro object, and return that
#remotePageObject = Pyro.core.getProxyForURI(URI)
remoteHashObject = Pyro.core.getProxyForURI(URI1)







#Functions to be called inside the service interface => This is the service interface
# can be called from the client, accessible to client (Chris)
#########################################################################################
#returns "g" if the data is ok to execute, and "b" if the data is bad => stop execution immediately



#submits a hashkey and unique identifiers to the service
def monitorSubmitHash(processData, arg1) :
	################################3
	#new code
	# connects to the scanner service ... hopefully
	locator = Pyro.naming.NameServerLocator()
	print 'Searching Name Server...',
	ns = locator.getNS()

	# resolve the Pyro object
	print 'finding object'
	repeats = arg1
	while repeats >= 0 :
		repeats = repeats -1
		try:
			URI=ns.resolve('clientHashTableInterfaceObj')
			print 'URI:',URI
			break
		except NamingError,x:
			print 'Couldn\'t find object, nameserver says:',x	
			sleepT = int(random.randrange(2, 10))
			time.sleep(sleepT)
			if repeats < 0: 
				raise SystemExit
		
	# create a proxy for the Pyro object, and return that
	clientHashTableInterfaceObj = Pyro.core.getProxyForURI(URI)
	print "blalbalablabalbalbblablablablablabla >" 

	print clientHashTableInterfaceObj.dummyy() #prints "hello andi the client side code works ... or at least does SOMETHING"
	#\new code
	################################3


	#check if it is in the local hash table first
	n = len(processData[0]) - 1
	while n >= 0 :
		#find each page from the pageList and compare to entries in hashTable

		#*********This will be affected if the data changes*********#
		hashClassObj = serviceHash.HashClass(processData[0][n], processData[1], processData[2], processData[3] )
		searchResults = clientHashTableInterfaceObj.searchHashList(hashClassObj)
		if  searchResults[0] : #page already exists intable
			print "object exists in LOCAL list already => WE HAVE A HIT, CONGRATULATIONS"
		#	if searchResults[1] == "b" :
			return searchResults[1]
		else : #page does NOT exists => entire process is new => need to ship everything off
			print "object does NOT exist in LOCAL list already"
			break
				#PageTableList is full
			
		n = n - 1


	#it looped through entire thing and none of the pages are bad
	#it is in hash table and it is good
	if n < 0 :
		print "I don't think this should happen when testing, right now ...."
		return "g"


	#it is not in the hash table, send it over the network for scanning 
	result = "0"
	repeats = arg1
	if arg1 > REPEATS_IF_CONN_DENIED :
		repeats = REPEATS_IF_CONN_DENIED
	sleepT = int(random.randrange(2, 10))	
	children = True
	
	while (result == "0" and repeats >= 0):
		repeats -= 1
			
		try :
			if children :
				try:
					os.wait()
				except:
					children = False
			print "the NORMAL ONE IS RUNNUNG"
			result = remoteHashObject.submitHash(processData)
			print "the NORMAL ONE IS  RETURNING"
			
				
		except ConnectionDeniedError, x:
			sleepT = sleepT * sleepT
			time.sleep(sleepT)
			print "we are here @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@2"
			result = remoteHashObject.submitHash(processData)

	print result, "this is the result in service_client" #just in here for sanity check
		#PageTableList is full
	

	clientHashTableInterfaceObj.masterCleanUp()
	hashClassObj.status = result
	print "this worked too"
	clientHashTableInterfaceObj.appendList(hashClassObj)
	print clientHashTableInterfaceObj.dummyy() #prints "hello andi the client side thing works"
	
#print ("LOCAL Hash Table Load Factor : " , (len(LocalHashTableList)/ 1.0 / LOCAL_HASH_TABLE_MAX_SIZE ))
	return result




#information: displays load factor of the according tables

def monitorHashTableLoadFactor() :
	return remoteHashObject.HashTableLoadFactor()








#class testit(Thread):
#   def __init__ (self, i):
#		Thread.__init__(self)
#		self.id = i
#		self.status = -1
#   def run(self):
#		result = 0
#		repeats = REPEATS_IF_CONN_DENIED
#		sleepT = int(random.randrange(2, 5))
#		children = True
#		while repeats >= 0 :
#			repeats -= 1
#				
#			try :
#				result =  monitorSubmitHash(pageDataObjectAllInOne)
#			except ConnectionDeniedError, x:
#				sleepT = sleepT * sleepT
#				if children:
#					try:
#						os.wait()
#					except:
#						children = False
#
#				time.sleep(sleepT)
#				print "we are here @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@2", self.id
#				result =  monitorSubmitHash(pageDataObjectAllInOne)
#			if result == "b" :
#				pass			
#				#print "********MALICIOUS CODE WAS FOUND, do what you need to do to fix it .....********"
#			elif result == "g":
#				pass
#				#print "********EVERYTHING IS OK: resume normal operation********"
#			else :
#				print "something has gone wrong, you shold never see this string"#
			#print self.id , "finished"
			#print monitorHashTableLoadFactor()
			
#		exit(0)

		
#processes = 0
#i = 0
#f = open("samplePage0","r")
#f1 = open("samplePage1", "r")
#f2 = open("samplePage2", "r")
#f2 = open("hugeFile", "r")
#f3 = f2.read()
#print monitorSubmitPage("hello120")


#page0 = f.read()
#page1 = f1.read()
#page2 = f2.read()
#while processes < NUMBER_OF_PROCESSES : #number of processes desired
#	processes += 1
#	child_pid = os.fork()
#	if child_pid == 0 : #inside the child
		# Pyro Stuff#
		#########################################################################################
		# locate the NS
#		locator = Pyro.naming.NameServerLocator()
	#	print 'Searching Name Server...',
#		ns = locator.getNS()
		# resolve the Pyro object
	#	print 'finding object'
#		try:
		#	URI=ns.resolve('remotePageObject')
		#	print 'URI:',URI
#			URI1=ns.resolve('remoteHashObject')
	#		print 'URI1:',URI1
#		except NamingError,x:
	#		print 'Couldn\'t find object, nameserver says:',x
#			raise SystemExit

		# create a proxy for the Pyro object, and return that
		#remotePageObject = Pyro.core.getProxyForURI(URI)
#		remoteHashObject = Pyro.core.getProxyForURI(URI1)
	
#		i = 0
#		while i < NUMBER_OF_THREADS : #number of threads desired
#			#print "it works " , str(random.randrange(0, 3000))#
#			whichChoice = random.randrange(0, 100)
#	#		print "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&=>" , whichChoice, "<="
#			if whichChoice < 10 :#
#				page0 = page0.replace('A', '4', random.randrange(0, 1000))#
#				page1 = page1.replace('W','5', random.randrange(0, 1000))
#				page1 = page2.replace('Y','2', random.randrange(0, 1000))
#				ageList = [page0, page1, page2]
#				processID = random.randrange(0,10000)#
#				processAge =  random.randrange(0,10000)
#				pageListDomainID =  random.randrange(0,20)
#	
#			elif whichChoice > 90:	
#				page0 = page0.replace('4','A', random.randrange(0, 1000))
#				page1.replace('5','W', random.randrange(0, 1000))
#				page2.replace('2','Y', random.randrange(0, 1000))
	#		print "length of the changed string " , len(page0)#
#			#print page1
		

	
	
			#THIS IS THE ONLY THING YOU NEED TO DO IN YOUR CODE
			#THEN CHECK THE RESULT FOR "g" OR "b" => "g" MEANS NOTHING MALICIOUS WAS FOUND; "b" MEANS stop executing, bad stuff was found
#			pageDataObjectAllInOne = [pageList,processID, processAge, pageListDomainID]
	
	
	
#			print "thread started" , i,  " of process ", os.getpid()
#			current = testit(i)
#			current.start()	
#			
#			i = i + 1
#	else :
	#	os.wait()	
	#	os.waitpid(child_pid, os.WNOHANG)
#		print "process finished : " , processes

#print "all finished"
		
#print monitorHashTableLoadFactor(), "is the load factor"
	
	
