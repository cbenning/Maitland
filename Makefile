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


##Rules
all:
	#for dir in $(DIRS); do make -C $$dir $@; done
	cd ./drivers/xen/malpage ; make
	cd ./drivers/xen/monitor ; make
	#cd ./drivers/xen/genshm-front ; make
	#cd ./drivers/xen/genshm-back ; make
	#cd ./drivers/xen/process_stop ; make
	#cd ./domU/c_target_sample ; make static
	cd ./domU/c_target_sample2 ; make static
	cd ./domU/c_loader ; make
	#cd ./packers/exepak-1.5 ; make	

clean: 
	cd ./drivers/xen/malpage ; make clean
	cd ./drivers/xen/monitor ; make clean
	#cd ./drivers/xen/genshm-front ; make clean
	#cd ./drivers/xen/genshm-back ; make clean
	#cd ./drivers/xen/process_stop ; make clean
	#cd ./domU/c_target_sample ; make clean
	cd ./domU/c_target_sample2 ; make clean
	cd ./domU/c_loader ; make clean
	#cd ./packers/exepak-1.5 ; make clean
