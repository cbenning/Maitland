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
GENSHMB_DEVICE_NAME = "genshm"
GENSHMB_XS_REGISTER_PATH = "/genshm/register"
GENSHMB_DEVICE = "/dev/"+GENSHMB_DEVICE_NAME


#Commands
GENSHMB_IOC_MAGIC = 270
GENSHMB_REPORT = GENSHMB_IOC_MAGIC+1
GENSHMB_REGISTER = GENSHMB_IOC_MAGIC+8
GENSHMB_DEREGISTER = GENSHMB_IOC_MAGIC+9
GENSHMB_MIN_DOMID = 1
GENSHMB_MAX_DOMID = 255

import fcntl, os, sys, time, struct,commands
sys.path.append("/usr/lib/xen-4.0/lib/python/")
from xen.xend.xenstore.xsutil import *
from xen.xend.xenstore.xswatch import *


class Genshmb():
    def __init__(self,fileName):
        self._filehandle = file(fileName,'r')

    def close(self):
        self._filehandle.close()
    
    def doGenshmbOp(self, cmd, pid):
        return fcntl.ioctl(self._filehandle, cmd, pid)

    def getDIDFromUUID(self,uuid):
        path = GENSHMB_XENSTORE_DOMID_PATH.replace(GENSHMB_XENSTORE_DOMID_PATH_REPLACE,uuid)
        return int(commands.getoutput("xenstore-read %s" % (path)))



def watch_domain_down(path, xs):

    #read the value, see if it's valid
    th = xs.transaction_start()    
    value = xs.read(th, path)
    xs.transaction_end(th)

    if value == None:

        print "Domain down, deregistering."
        ops = Genshmb(GENSHMB_DEVICE)
        ops.doGenshmbOp(GENSHMB_DEREGISTER, domid)
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

        ops = Genshmb(GENSHMB_DEVICE)
        procStruct = struct.pack("IIIs",int(values[0]),int(values[1]),int(values[2]),str(values[3]))
        ops.doGenshmbOp(GENSHMB_REGISTER, procStruct)
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
        register_path = GENSHMB_XS_REGISTER_PATH + "/"+ str(value)
        
        print "Creating "+register_path
        xs.mkdir(th,register_path)
        
        #setup watch on new directory
        xswatch(register_path, watch_domain_register, xs)
        
        #set perms of new directory: set_permissions takes a list of three tuples
        perm_tuple = { "dom":int(value), "read":True , "write":True }
        xs.set_permissions(th,register_path, [perm_tuple,perm_tuple,perm_tuple])
        xs.transaction_end(th)
        
        #xs.write(th,path_1,arg_1)
        #xs.write(th,path_2,arg_2)
        #xs.write(th,path_3,arg_3)
        #xs.write(th,path_4,arg_4)
        
        
        # Write backend information into the location the frontend will look for it.        
        #commands.getstatusoutput("xenstore-write /local/domain/"+str(value)+"/device/"+GENSHMB_DEVICE_NAME+"/0/backend-id 0")
        #commands.getstatusoutput("xenstore-write /local/domain/"+str(value)+"/device/"+GENSHMB_DEVICE_NAME+"/0/backend /local/domain/0/backend/"+GENSHMB_DEVICE_NAME+"/"+str(value)+"/0")
        # Write frontend information into the location the backend will look
        # for it.
        #commands.getstatusoutput("xenstore-write /local/domain/0/backend/"+GENSHMB_DEVICE_NAME+"/"+str(value)+"/0/frontend-id "+str(value))
        #commands.getstatusoutput("xenstore-write /local/domain/0/backend/"+GENSHMB_DEVICE_NAME+"/"+str(value)+"/0/frontend /local/domain/"+str(value)+"/device/"+GENSHMB_DEVICE_NAME+"/0")
        
        # Set the permissions on the backend so that the frontend can
        # actually read it.        
        #commands.getstatusoutput("xenstore-chmod /local/domain/"+str(value)+"/device/"+GENSHMB_DEVICE_NAME+"/0 r")
        #commands.getstatusoutput("xenstore-chmod /local/domain/0/backend/"+GENSHMB_DEVICE_NAME+"/"+str(value)+"/0 r")
        # Write the states.  Note that the backend state must be written
        # last because it requires a valid frontend state to already be
        # written.
        #commands.getstatusoutput("xenstore-write /local/domain/"+str(value)+"/device/"+GENSHMB_DEVICE_NAME+"/0/state 1")
        #commands.getstatusoutput("xenstore-write /local/domain/0/backend/"+GENSHMB_DEVICE_NAME+"/"+str(value)+"/0/state 1")
        


        #remove boot-up watch
        return False

    
    return True


def clean(xs):
    
    th = xs.transaction_start()
    print "Removing: "+GENSHMB_XS_REGISTER_PATH    
    xs.rm(th,GENSHMB_XS_REGISTER_PATH)
    #for i in range(GENSHMB_MIN_DOMID, GENSHMB_MAX_DOMID):
        #print "Removing: "+GENSHMB_XS_REGISTER_PATH + "/"+ str(i)    
        #xs.rm(th,GENSHMB_XS_REGISTER_PATH + "/"+ str(i))

    xs.transaction_end(th)       
        #commands.getstatusoutput("xenstore-write /local/domain/"+str(i)+"/device/"+GENSHMB_DEVICE_NAME)
        #commands.getstatusoutput("xenstore-write /local/domain/0/backend/"+GENSHMB_DEVICE_NAME+"/"+str(i))
        
        

def main():


    
    xs = xshandle()

    if len(sys.argv) > 1 and sys.argv[1]=='clean':
        clean(xs)
        return

    th = xs.transaction_start()    
    xs.mkdir(th,GENSHMB_XS_REGISTER_PATH)
    xs.transaction_end(th)
        
    for i in range(GENSHMB_MIN_DOMID, GENSHMB_MAX_DOMID):
        path = xs.get_domain_path(i) + "/domid"
        #print "watching "+path
        xswatch(path, watch_domain_up, xs)

    while(True):
        time.sleep(10000)



main()