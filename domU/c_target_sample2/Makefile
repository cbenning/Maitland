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

all:
	#gcc -Wall -o runme main.c 
	gcc -Wall -o runme2 main2.c 
    
static:
	#gcc -Wall --static -o runme main.c
	gcc -Wall --static -o runme2 main2.c
	cp runme2 runme2_upx
	cp runme2 runme2_gzexe
	upx runme2_upx
	gzexe runme2_gzexe
	rm runme2_gzexe~
	
clean:
	#rm runme
	rm runme2
	#rm runme_p
	rm runme2_upx
	rm runme2_gzexe
	
