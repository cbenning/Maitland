
##This information is old!!!!

To get the kernel to compile the kernel modules

In drivers/xen/Makefile, need to add:
obj-$(CONFIG_MALPAGE)   += malpage/
obj-$(CONFIG_MONITOR)   += monitor/

In drivers/xen/Kconfig, need to add:
config MALPAGE
        tristate "DomU malpage module"
        depends on XEN
        default y
        help

config MONITOR
        tristate "Dom0 malpage monitor module"
        depends on XEN
        default y
        help
