#!/usr/bin/python

import sys,binascii

def search(prog, string):
        print "Searchstring: "+str(string)
        print "Filename: "+str(prog)

        index = 1096
        f1 = open(prog,"rb")
        new_chunk = f1.read(index)
        str1 = ''
        str2 = ''
        while(new_chunk):
            str1 = str2
            str2 = new_chunk
            combined = str(str1+str2)
            tmp = binascii.a2b_qp(string)
            if(str.find(combined,tmp)>=0):
                print "Found searchstring"
                break
            print "Advancing to byte "+str(index)
            f1.seek(index)
            index += index
            new_chunk = f1.read(1096)
        f1.close()


search(sys.argv[1],sys.argv[2])
