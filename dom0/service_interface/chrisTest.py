#!/usr/bin/python

import Pyro.naming, Pyro.core
from Pyro.errors import NamingError
from Pyro.errors import ConnectionDeniedError
import os.path
import serviceHash
from serviceHashFunction import myServiceHashFn
import service_client
#for testing
from threading import Thread
import random
import time
import os
import types



#########################################################################################
#testing (this would be the code that the remote clients would use to call stuff)
#i.e. the code inside your module could use a variation of the code below
#########################################################################################
print "################################START############################"
f = open("samplePage0","r")
f1 = open("samplePage1", "r")
f2 = open("samplePage2", "r")
#f2 = open("hugeFile", "r")
#f3 = f2.read()
#print monitorSubmitPage("hello120")


page0 = f.read()
page1 = f1.read()
page2 = f2.read()

processID = 10000
processAge = 2000
pageListDomainID = 3



#this while loop is only here to test the local hash table (set it to >1 to actually test the local hash table)
n = 1
while n > 0:
	print "we are here " , n
	n -= 1


	#this chunck only pseudo randomizes the input data a little bit
	whichChoice = random.randrange(0, 100)
	#whichChoice = 39
	#	#		print "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&=>" , whichChoice, "<="
	if whichChoice < 10 :#
		page0 = page0.replace('A', '4', random.randrange(0, 1000))#
		page1 = page1.replace('W','5', random.randrange(0, 1000))
		page1 = page2.replace('Y','2', random.randrange(0, 1000))
		ageList = [page0, page1, page2]
		processID = random.randrange(0,10000)
		processAge =  random.randrange(0,10000)
		pageListDomainID =  random.randrange(0,20000)
	
	elif whichChoice > 90:	
		page0 = page0.replace('4','A', random.randrange(0, 1000))
		page1.replace('5','W', random.randrange(0, 1000))
		page2.replace('2','Y', random.randrange(0, 1000))
		processID = random.randrange(0,10000)
		processAge =  random.randrange(0,10)
		pageListDomainID =  random.randrange(0,20)
		#		print "length of the changed string " , len(page0)#
	#			#print page1


	pageList = [page0, page1, page2]


#############################################################################################################################
#THIS IS THE ONLY THING YOU NEED TO DO IN YOUR CODE
#THEN CHECK THE RESULT FOR "g" OR "b" => "g" MEANS NOTHING MALICIOUS WAS FOUND; "b" MEANS stop executing, bad stuff was found
	pageDataObjectAllInOne = [pageList,processID, processAge, pageListDomainID]

	result =  service_client.monitorSubmitHash(pageDataObjectAllInOne, 10)
	if result == "b" :
		print "********MALICIOUS CODE WAS FOUND, do what you need to do to fix it .....********"
	elif result == "g":
		print "********EVERYTHING IS OK: resume normal operation********"
	else :
		print "something has gone wrong, you shold never see this string"

#############################################################################################################################
	
		#YOU DO NOT NEED THESE, THEY ARE JUST CONVENIENT TO HAVE
	print service_client.monitorHashTableLoadFactor()

	#IGNORE: THEY ARE JUST FOR TESTING STUFF OUT
print "this is the hash built in :" , hash(107676776776767475487654685687632847632846934239877600000011122)
print "this is the hash my Fn    :" , myServiceHashFn(107676776776767475487654685687632847632846934239877600000011122)
#print "this is the hash" , hash(f2.read())
print "################################STOP#############################"

		
print service_client.monitorHashTableLoadFactor(), "is the load factor"
	
	
