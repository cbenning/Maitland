
##Rules
all:
	#for dir in $(DIRS); do make -C $$dir $@; done
	cd ./drivers/xen/malpage ; make
	cd ./drivers/xen/monitor ; make
	cd ./drivers/xen/genshm-front ; make
	cd ./drivers/xen/genshm-back ; make
	#cd ./drivers/xen/process_stop ; make
	cd ./domU/c_target_sample ; make static
	cd ./domU/c_target_sample2 ; make static
	cd ./domU/c_loader ; make

clean: 
	cd ./drivers/xen/malpage ; make clean
	cd ./drivers/xen/monitor ; make clean
	cd ./drivers/xen/genshm-front ; make clean
	cd ./drivers/xen/genshm-back ; make clean
	#cd ./drivers/xen/process_stop ; make clean
	cd ./domU/c_target_sample ; make clean
	cd ./domU/c_target_sample2 ; make clean
	cd ./domU/c_loader ; make clean
