#!/usr/bin/python

'''
     XSPY_METH(read,              METH_VARARGS),
 853     XSPY_METH(write,             METH_VARARGS),
 854     XSPY_METH(ls,                METH_VARARGS),
 855     XSPY_METH(mkdir,             METH_VARARGS),
 856     XSPY_METH(rm,                METH_VARARGS),
 857     XSPY_METH(get_permissions,   METH_VARARGS),
 858     XSPY_METH(set_permissions,   METH_VARARGS),
 859     XSPY_METH(watch,             METH_VARARGS),
 860     XSPY_METH(read_watch,        METH_NOARGS),
 861     XSPY_METH(unwatch,           METH_VARARGS),
 862     XSPY_METH(transaction_start, METH_NOARGS),
 863     XSPY_METH(transaction_end,   METH_VARARGS | METH_KEYWORDS),
 864     XSPY_METH(introduce_domain,  METH_VARARGS),
 865     XSPY_METH(set_target,        METH_VARARGS),
 866     XSPY_METH(resume_domain,     METH_VARARGS),
 867     XSPY_METH(release_domain,    METH_VARARGS),
 868     XSPY_METH(close,             METH_NOARGS),
 869     XSPY_METH(get_domain_path,   METH_VARARGS),
'''


#Driver
MONITOR_DEVICE_NAME = "monitor"
MONITOR_XS_REGISTER_PATH = "/malpage/register"
MONITOR_DEVICE = "/dev/"+MONITOR_DEVICE_NAME


#Commands
MONITOR_IOC_MAGIC = 270
MONITOR_REPORT = MONITOR_IOC_MAGIC+1
MONITOR_REGISTER = MONITOR_IOC_MAGIC+8
MONITOR_DEREGISTER = MONITOR_IOC_MAGIC+9
MONITOR_MIN_DOMID = 1
MONITOR_MAX_DOMID = 255

import fcntl, os, sys, time, struct,commands
sys.path.append("/usr/lib/xen-4.0/lib/python/")
from xen.xend.xenstore.xsutil import *
from xen.xend.xenstore.xswatch import *


class Monitor():
    def __init__(self,fileName):
        self._filehandle = file(fileName,'r')

    def close(self):
        self._filehandle.close()
    
    def doMonitorOp(self, cmd, pid):
        return fcntl.ioctl(self._filehandle, cmd, pid)


def watch_domain_down(path, xs):

    #read the value, see if it's valid
    th = xs.transaction_start()    
    value = xs.read(th, path)
    xs.transaction_end(th)

    if value == None:

        print "Domain down, deregistering."
        ops = Genshmb(MONITOR_DEVICE)
        ops.doMonitorOp(MONITOR_DEREGISTER, domid)
        ops.close()

        xswatch(path, watch_domain_up, xs)
        return False


    return True


def watch_domain_register(path, xs):

    #read the value, see if it's valid
    th = xs.transaction_start()    
    value = xs.read(th, path)
    xs.transaction_end(th)

    if (len(value) > 0):

        #delete the node
        th = xs.transaction_start()    
        print "Removing "+path 
        xs.rm(th,path)
        xs.transaction_end(th)

        #notify the Kmod
        print "Sending registration to Kmod: "+value
        values = value.split(":")

        ops = Monitor(MONITOR_DEVICE)
        procStruct = struct.pack("IIIs",int(values[0]),int(values[1]),int(values[2]),str(values[3]))
        ops.doMonitorOp(MONITOR_REGISTER, procStruct)
        ops.close()

        print "Domain "+str(value)+" registered"

        watch_path = xs.get_domain_path(int(values[0])) + "/domid"
        xswatch(watch_path, watch_domain_down, xs)
        
        #remove the watch
        return False
        
    
    return True



def watch_domain_up(path, xs):

    #read the domid, see if it's valid
    th = xs.transaction_start()    
    value = xs.read(th, path)
    xs.transaction_end(th)

    if (int(value) > 0):
    
        print "Domain "+str(value)+" detected"

        #setup registration directory for this domid in /malpage/register
        th = xs.transaction_start()    
        register_path = MONITOR_XS_REGISTER_PATH + "/"+ str(value)
        
        print "Creating "+register_path
        xs.mkdir(th,register_path)
        
        #setup watch on new directory
        xswatch(register_path, watch_domain_register, xs)
        
        #set perms of new directory: set_permissions takes a list of three tuples
        perm_tuple = { "dom":int(value), "read":True , "write":True }
        xs.set_permissions(th,register_path, [perm_tuple,perm_tuple,perm_tuple])
        xs.transaction_end(th)

        #remove boot-up watch
        return False

    
    return True


def clean(xs):
    
    print "Removing: "+MONITOR_XS_REGISTER_PATH    
    th = xs.transaction_start()
    xs.rm(th,MONITOR_XS_REGISTER_PATH)
    xs.transaction_end(th)       


def main():


    
    xs = xshandle()

    if len(sys.argv) > 1 and sys.argv[1]=='clean':
        clean(xs)
        return

    th = xs.transaction_start()    
    xs.mkdir(th,MONITOR_XS_REGISTER_PATH)
    xs.transaction_end(th)
        
    for i in range(MONITOR_MIN_DOMID, MONITOR_MAX_DOMID):
        path = xs.get_domain_path(i) + "/domid"
        #print "watching "+path
        xswatch(path, watch_domain_up, xs)

    while(True):
        time.sleep(10000)



main()