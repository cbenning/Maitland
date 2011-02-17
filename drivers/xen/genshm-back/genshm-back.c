/******************************************************************************
 * monitor.c
 *
 * Monitor driver - domU driver for reporting processes given by malpage driver
 *
 * Copyright (c) 2010 Christopher Benninger.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation; or, when distributed
 * separately from the Linux kernel or incorporated into other
 * software packages, subject to the following license:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this source file (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

//Linux includes
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <asm/pgtable_types.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <asm/uaccess.h>

//Userlevel includes
#include <linux/string.h>    //for join()

//Xen includes
#include <xen/interface/platform.h>
#include <xen/grant_table.h>
#include <xen/interface/io/ring.h>
#include <xen/interface/xen.h>
#include <xen/interface/elfnote.h>
//#include <xen/driver_util.h>
//#include <xen/gnttab.h>
#include <xen/xenbus.h>
#include <xen/events.h>
#include <config/sys/hypervisor.h>
#include <asm/hypervisor.h>
#include <linux/slab.h>

//Custom includes
#include "genshm-back.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("CHRISBENNINGER");
MODULE_ALIAS("genshmb");



/************************************************************************

Interface and Util Functions

************************************************************************/

static int genshmb_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg) {
	
	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "->genshmb_ioctl\n");
	printk(KERN_ALERT "->genshmb_ioctl: command %d.\n", cmd);
	#endif
	
	switch(cmd){
		case GENSHMB_REGISTER:
			#ifdef GENSHMB_DEBUG
			printk(KERN_ALERT "Registering domain.\n");
			#endif
			genshm_back_info = genshmb_populate_info(arg);
			genshmb_register(genshm_back_info);
			break;

		case GENSHMB_DEREGISTER:
			#ifdef GENSHMB_DEBUG
			printk(KERN_ALERT "Deregistering domain.\n");
			#endif
			//monitor_register((( monitor_uspace_info_t*)arg));
			break;

		default:
			#ifdef GENSHMB_DEBUG
			printk(KERN_ALERT "Command not recognized: %d\n", cmd);
			#endif
			return GENSHMB_BADCMD;
			
	}

	return GENSHMB_SUCCESS;
}


static struct genshmb_info* genshmb_populate_info(unsigned long arg){

	struct genshmb_uspace* tmp_info;
	struct genshmb_info *info;

	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "->genshmb_populate_info");
	#endif

	tmp_info = (struct genshmb_uspace*)arg;
	info = kzalloc(sizeof(struct genshmb_info),GFP_KERNEL);

	//info->uuid = kmalloc(GENSHMB_UUID_LENGTH,0);
	//info->uuid = tmp_info->uuid;
	info->domid = tmp_info->domid;
	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "->genshmb_populate_info: domid: %u",info->domid);
	#endif

	info->gref = tmp_info->gref;
	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "->genshmb_populate_info: gref: %u",info->gref);
	#endif

	info->evtchn = tmp_info->evtchn;
	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "->genshmb_populate_info: evtchn: %u",info->evtchn);
	#endif


	return info;
}



/************************************************************************

Grant table and Interdomain Functions

************************************************************************/


static int genshmb_init(void){

	struct device *err_dev;
	int result = 0;


	printk(KERN_ALERT "->genshmb_init\n");

	if (!xen_domain())
		return -ENODEV;

	/*
	if (register_blkdev(genshmf_major, DEVICE_NAME)) {
		printk(KERN_WARNING "->genshmf_init: can't get major %d with name %s\n", genshmf_major, DEVICE_NAME);
		return -ENODEV;
	}
	*/

	printk(KERN_ALERT "->genshmb_init: Loading...\n");

	//Reserve a major number
	result = alloc_chrdev_region(&genshmb_dev, GENSHMB_MIN_MINORS, GENSHMB_MAX_MINORS, DEVICE_NAME);
	genshmb_major = MAJOR(genshmb_dev);
	genshmb_minor = MINOR(genshmb_dev);

	if (genshmb_major < 0) {
		printk(KERN_ALERT "->genshmb_init: Registering the character device failed with major number: %d, minor: %d", genshmb_major,genshmb_minor);
		return -ENODEV;
	}

	genshmb_class = class_create(THIS_MODULE, DEVICE_NAME);

    /* Connect the file operations with the cdev */
	cdev_init(&genshmb_cdev, &genshmb_fops);
	genshmb_cdev.owner = THIS_MODULE;


	/* Connect the major/minor number to the cdev */
	if (cdev_add(&genshmb_cdev, genshmb_dev, 1)) {
		printk(KERN_ALERT "->genshmb_init: Failed with registering the character device");
		return 1;
	}

	//Much simpler, but required udev to run on the machine
	err_dev = device_create(genshmb_class, NULL,genshmb_dev,"%s",DEVICE_NAME);

	if (err_dev == NULL) {
		printk(KERN_ALERT "->genshmb_init: Registering the character device failed with error: %d",result);
		return -ENODEV;
	}

	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "I was assigned major number %d\n", genshmb_major);
	printk(KERN_ALERT "PAGE_SHIFT: %d\n", PAGE_SHIFT);
	printk(KERN_ALERT "PAGE_SIZE: %lu\n", PAGE_SIZE);
	printk(KERN_ALERT "gref_list_t size: %lu\n", sizeof(gref_list_t));
	#endif

	//genshmb_xenbus_init();

	printk(KERN_ALERT "->genshmb_init: Loaded.\n");

	return 0;
}


static void genshmb_exit(void){

	printk(KERN_ALERT "->genshmb_exit\n");

	//Unregister the device

	//genshmb_xenbus_exit();

	device_destroy(genshmb_class,genshmb_dev);
	cdev_del(&genshmb_cdev);
	class_destroy(genshmb_class);
	//unregister_chrdev(genshmb_major, DEVICE_NAME);

}

//////////////////////////////////////////////////////////////////////////////
/**
 * Entry point to this code when a new device is created.  Allocate the basic
 * structures and switch to InitWait.
 */
static int genshmb_probe(struct xenbus_device* dev, const struct xenbus_device_id* id){

	printk(KERN_ALERT "->genshmb_probe\n");
	xenbus_switch_state(dev, XenbusStateInitialising);
	genshm_back_info = kzalloc(sizeof(genshm_back_info),GFP_KERNEL);
	genshm_back_info->xbdev = dev;
	printk(KERN_ALERT "->Backend State: XenbusStateInitWait\n");
	xenbus_switch_state(dev, XenbusStateInitWait);
	return 0;

}


static int genshmb_register(struct genshmb_info *info){

	int err;
	struct vm_struct *v_start;
	struct gnttab_map_grant_ref ops;
	struct gnttab_unmap_grant_ref unmap_ops;
	struct genshm_sring *sring;

	//info = kmalloc(sizeof(struct genshmb_info),0);

	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "->genshmb_register");
	#endif

	if(!info){
		printk(KERN_ALERT "Info struct not properly initialized\n");
		return GENSHMB_ALLOCERR;
	}

	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "->genshmb_register: got gref:%u domid:%u\n",info->gref,info->domid);
	#endif

	// The following function reserves a range of kernel address space and
	// allocates pagetables to map that range. No actual mappings are created.
	v_start = alloc_vm_area(PAGE_SIZE);
	if (v_start == 0) {
		free_vm_area(v_start);
		printk(KERN_ALERT "->genshmb_register: could not allocate page\n");
		return -EFAULT;
	}


	/*
	 ops struct in paramaeres
		host_addr, flags, ref
	 ops struct out parameters
		status (zero if OK), handle (used to unmap later), dev_bus_addr
	*/

	//Map in the remote page
	gnttab_set_map_op(&ops, (unsigned long)v_start->addr, GNTMAP_host_map, info->gref, info->domid);

	if (HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, &ops, 1)) {
		printk(KERN_ALERT "->genshmb_register: HYPERVISOR map grant ref failed\n");
		return -EFAULT;
	}
	if (ops.status) {
		/*
		#define GNTST_okay             (0)  // Normal return.
		#define GNTST_general_error    (-1) // General undefined error.
		#define GNTST_bad_domain       (-2) // Unrecognsed domain id.
		#define GNTST_bad_gntref       (-3) // Unrecognised or inappropriate gntref.
		#define GNTST_bad_handle       (-4) // Unrecognised or inappropriate handle.
		#define GNTST_bad_virt_addr    (-5) // Inappropriate virtual address to map.
		#define GNTST_bad_dev_addr     (-6) // Inappropriate device address to unmap.
		#define GNTST_no_device_space  (-7) // Out of space in I/O MMU.
		#define GNTST_permission_denied (-8) // Not enough privilege for operation.
		#define GNTST_bad_page         (-9) // Specified page was invalid for op.
		#define GNTST_bad_copy_arg    (-10) // copy arguments cross page boundary
			*/
		printk(KERN_ALERT "->genshmb_register: HYPERVISOR map grant ref failed status = %d\n", ops.status);
		return -EFAULT;

	}
	//printk("\nxen: dom0: shared_page = %x, handle = %x, status = %x", (unsigned int)v_start->addr, ops.handle, ops.status);
	// Used for unmapping
	unmap_ops.host_addr = (unsigned long)(v_start->addr);
	unmap_ops.handle = ops.handle;

	//Get a handle on the ring sitting in the page
	sring = (struct genshm_sring*)v_start->addr;
	BACK_RING_INIT(&(info->bring), sring, PAGE_SIZE);

	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "->genshmb_register: Back ring initialized\n");
	#endif

	//Setup an event channel to the frontend
	err = bind_interdomain_evtchn_to_irqhandler(info->domid, info->evtchn, genshmb_interrupt_handler, 0, GENSHMB_CHANNEL_NAME, info);

	if (err < 0) {
		#ifdef GENSHMB_DEBUG
		printk(KERN_ALERT "->genshmb_register: failed binding to evtchn with err: %d\n",err);
		#endif
		err = HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, &unmap_ops, 1);
		return -EFAULT;
	}

	//monitor_share_info->irq = err;
	info->evtchn = err;

	#ifdef GENSHMB_DEBUG
	printk(KERN_ALERT "->genshmb_register: done\n");
	#endif

	return 0;
}



static int genshmb_connect_ring(struct genshmb_info *be)
{
	struct xenbus_device *dev = be->xbdev;
	unsigned long gref;
	unsigned int evtchn;
	int err;
	struct gnttab_map_grant_ref ops;
	struct gnttab_unmap_grant_ref unmap_ops;
	struct genshm_sring *sring;
	struct vm_struct *v_start;

	err = xenbus_gather(XBT_NIL, dev->otherend, "ring-ref", "%lu", &gref,"event-channel", "%u", &evtchn, NULL);
	if (err) {
		xenbus_dev_fatal(dev, err,"reading %s/ring-ref and event-channel",dev->otherend);
		return err;
	}

	printk(KERN_INFO "->genshmb_connect_ring: gref %ld, event-channel %d\n",gref, evtchn);

	be->gref = gref;
	be->evtchn = evtchn;

	// The following function reserves a range of kernel address space and
	// allocates pagetables to map that range. No actual mappings are created.
	v_start = alloc_vm_area(PAGE_SIZE);
	if (v_start == NULL) {
		free_vm_area(v_start);
		printk(KERN_ALERT "->genshmb_connect_ring: could not allocate page\n");
		return -EFAULT;
	}

	be->ring_area = v_start;
	gnttab_set_map_op(&ops, (unsigned long)v_start->addr, GNTMAP_host_map, be->gref, be->xbdev->otherend_id);

	if (HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, &ops, 1))
		BUG();

	if (ops.status) {
		/*
		#define GNTST_okay             (0)  // Normal return.
		#define GNTST_general_error    (-1) // General undefined error.
		#define GNTST_bad_domain       (-2) // Unrecognsed domain id.
		#define GNTST_bad_gntref       (-3) // Unrecognised or inappropriate gntref.
		#define GNTST_bad_handle       (-4) // Unrecognised or inappropriate handle.
		#define GNTST_bad_virt_addr    (-5) // Inappropriate virtual address to map.
		#define GNTST_bad_dev_addr     (-6) // Inappropriate device address to unmap.
		#define GNTST_no_device_space  (-7) // Out of space in I/O MMU.
		#define GNTST_permission_denied (-8) // Not enough privilege for operation.
		#define GNTST_bad_page         (-9) // Specified page was invalid for op.
		#define GNTST_bad_copy_arg    (-10) // copy arguments cross page boundary
		*/
		printk(KERN_ALERT "->genshmb_connect_ring: Grant table operation failure !\n");
		return ops.status;
	}

	//Get a handle on the ring sitting in the page
	sring = (struct genshm_sring*)v_start->addr;
	BACK_RING_INIT(&genshm_back_info->bring, sring, PAGE_SIZE);

	err = bind_interdomain_evtchn_to_irqhandler(be->xbdev->otherend_id, be->evtchn, genshmb_interrupt_handler, 0, GENSHMB_CHANNEL_NAME, be);
	if (err < 0){

		unmap_ops.host_addr = (unsigned long)(v_start->addr);
		unmap_ops.handle = ops.handle;

		gnttab_set_unmap_op(&unmap_ops,unmap_ops.host_addr, GNTMAP_host_map, unmap_ops.handle);

		if (HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, &unmap_ops, 1))
			BUG();

		free_vm_area(v_start);
		printk(KERN_ALERT "->genshmb_connect_ring: Dying miserably...\n");
		be->irq = err;
		return err;
	}

	return 0;
}

static int genshmb_disconnect_ring(struct genshmb_info *be){

	struct gnttab_unmap_grant_ref unmap_ops;

	if (be->irq) {
		unbind_from_irqhandler(be->irq, be);
		be->irq = 0;
	}

	if (be->bring.sring) {

		gnttab_set_unmap_op(&unmap_ops,unmap_ops.host_addr, GNTMAP_host_map, unmap_ops.handle);

		if (HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, &unmap_ops, 1))
			BUG();

		free_vm_area(be->ring_area);
		be->bring.sring = NULL;
	}

}

//////////////////////////////////////////////////////////////////////////////
static int genshmb_remove(struct xenbus_device *dev){
	printk(KERN_ALERT "->genshmb_remove\n");
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
/**
 * Callback received when the frontend's state changes.
 */
static void genshmf_changed(struct xenbus_device *dev, enum xenbus_state frontend_state){

	int err;
	//struct genshmf_info *info = dev_get_drvdata(&dev->dev);
	printk(KERN_ALERT "->genshmf_changed: frontend changed to state %s.\n", xenbus_strstate(frontend_state));

	/*
	XenbusStateUnknown      = 0,
	xenbusStateInitialising = 1,
	XenbusStateInitWait     = 2,  // Finished early initialisation, but waiting for information from the peer or hotplug scripts.
	XenbusStateInitialised  = 3,  // Initialised and waiting for a connection from the peer.
	XenbusStateConnected    = 4,
	XenbusStateClosing      = 5,  // The device is being closed due to an error or an unplug event.
	XenbusStateClosed       = 6
	*/

	switch (frontend_state) {
		case XenbusStateInitialising:
			if (dev->state == XenbusStateClosed) {
				xenbus_switch_state(dev, XenbusStateInitWait);
			}
			break;
		case XenbusStateInitWait:
			break;
		case XenbusStateClosed:
			xenbus_switch_state(dev, XenbusStateClosed);
			break;
			//if (xenbus_dev_is_online(dev))
			//	break;
		case XenbusStateUnknown:
			//device_unregister(&dev->dev);
			break;
		case XenbusStateReconfiguring:
			break;
		case XenbusStateReconfigured:
			break;
		case XenbusStateInitialised:
		case XenbusStateConnected:
			if (dev->state == XenbusStateConnected)
				break;
			//genshmb_connect_ring(genshm_back_info);
			xenbus_switch_state(dev, XenbusStateConnected);
			printk(KERN_ALERT "->genshmb: Ready.\n", xenbus_strstate(frontend_state));
			break;
		case XenbusStateClosing:
			//genshmb_disconnect_ring(genshm_back_info);
			xenbus_switch_state(dev, XenbusStateClosing);
			break;
		}
}


static irqreturn_t genshmb_interrupt_handler(int irq, void *dev_id){

	struct genshmb_info *info = (struct genshmb_info *)dev_id;

	printk(KERN_ALERT "->genshmb_interrupt_handler\n");
	//tasklet_schedule(&info->tasklet);

	return IRQ_HANDLED;
}


static int genshmb_uevent(struct xenbus_device* xdev, char** envp, int num_envp, char* buffer, int buffer_size){
        return 0;
}


//////////////////////////////////////////////////////////////////////////////
void genshmb_xenbus_init(){
	printk(KERN_ALERT "->genshmb_xenbus_init\n");
	//xenbus_register_backend(&genshmb_driver);
}

void genshmb_xenbus_exit(){

	//xenbus_unregister_driver(&genshmb_driver);
}


module_init( genshmb_init);
module_exit( genshmb_exit);
