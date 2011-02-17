#DEV_LIBS=/lib/modules/2.6.31.10-xen-dev/build
#DEV_LIBS=/home/chris/workspace/dev_lib/modules/2.6.31.10-xen-dev/build
#DEV_LIBS=/home/chris/workspace/dev_lib/modules/2.6.32.27/source
#DEV_LIBS=/home/chris/workspace/dev_lib/modules/linux-2.6.32-xen
#DEV_LIBS=/home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git
DEV_LIBS=/home/chris/workspace/dev_lib/modules/build-linux-2.6-pvops_x86_64

obj-m := monitor.o

all:
	make -C $(DEV_LIBS) M=$(PWD) modules

clean:
	make -C $(DEV_LIBS) M=$(PWD) clean