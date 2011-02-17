#!/usr/bin/python
import Pyro.core, Pyro.naming
import Service_Objects

from Pyro.errors import PyroError, NamingError

__author__="Andi"
__version__ = "$Revision$"

###### PageClass Pyro object

class HashClassInService(Pyro.core.SynchronizedObjBase, Service_Objects.HashClassInService):
    """Composion of the hash class and pyro"""
    pass


import optparse
import sys

options = optparse.Values(defaults={'verbose':False, 'remote':False,
                                    'logname':'~/comm_delay/'})

def parse_commandline(values = None, args = sys.argv[1:]):
    """Take the program arguments OR a string and get our command line options
    out of them.
    @param args: and optional string with arguments in it.
    @param values: a optparse Values object to populate.
    @return: the optparse database of options found.  """
    
    usage = "usage: %prog [options] arg1 arg2"
    
    testrun = False
    if not (args is sys.argv[1:]):
        testrun = True

    if values is None:
        values = options
    my_parser = optparse.OptionParser(usage=usage)
    #note: default options are in the values structure not set by these calls
    my_parser.add_option("-v", "--verbose", action="store_true", 
                         dest="verbose", 
                         help='Run the program with verbose output.')

    my_parser.add_option("-a", "--address",  dest="address", 
                         help="The local IP address")
    my_parser.process_default_values = True


    args = my_parser.parse_args(values=values, args = args)[1]
    
    #if there are no args, print the help
    if not testrun and (len(sys.argv) < 2) or len(args)<0:
        my_parser.print_help()
        sys.exit(2)
    
    return values

thisMachinesIPaddress = "192.168.0.205"


def main():
    """main server program"""
    parse_commandline();
    Pyro.config.PUBLIC_HOST = options.address   #NOTE: this should not be hardcoded in final version
    Pyro.core.initServer()
    daemon = Pyro.core.Daemon(host=options.address)
	
    # locate the NS
    locator = Pyro.naming.NameServerLocator()
    print 'searching for Name Server...'
    ns = locator.getNS()
    daemon.useNameServer(ns)
    # connect a new object implementation (first unregister previous one)
    try:
 	  # 'remoteHashObject' is the name by which our object will be known to the outside world
       ns.unregister('remoteHashObject')
    except NamingError:
       pass
	# connect new object implementation
    daemon.connect(HashClassInService(), 'remoteHashObject')
	

    # enter the server loop.
    #print 'Server object "remotePageObject" ready.'
    print 'Server object "remoteHashObject" ready.'
    URITest = ns.resolve('remoteHashObject')
    print 'URITest:', URITest
    daemon.requestLoop()
	#connect to server - upwards (the elec guy side)

	# connects to the scanner service ... hopefully
    locator = Pyro.naming.NameServerLocator()
    print 'Searching Name Server...',
    ns = locator.getNS()
	# resolve the Pyro object
    print 'finding object'
    try:
        URI = ns.resolve('scannerInterfaceObj')
        print 'URI:', URI
        print "hello andi"
    except NamingError, x:
		print 'Couldn\'t find object, nameserver says:', x
		raise SystemExit

	# create a proxy for the Pyro object, and return that
    scannerInterfaceObj = Pyro.core.getProxyForURI(URI)


if __name__ == "__main__":
		main()


