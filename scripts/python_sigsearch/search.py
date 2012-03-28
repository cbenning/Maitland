#!/usr/bin/python
# Maitland: A prototype paravirtualization-based packed malware detection system for Xen virtual machines
# Copyright (C) 2011 Christopher A. Benninger

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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
