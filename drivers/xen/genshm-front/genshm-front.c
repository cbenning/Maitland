/******************************************************************************
 * malpage.c
 *
 * Malpage driver - domU driver for exporting process information
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
 ******************************************************************************/

//Linux Includes
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
#include <linux/proc_fs.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/sync_bitops.h>

//Xen Includes
#include <xen/xenbus.h>
#include <xen/interface/platform.h>
#include <xen/interface/xen.h>
#include <xen/interface/io/ring.h> 
#include <xen/grant_table.h>
#include <xen/page.h>
#include <xen/interface/callback.h>
#include <xen/events.h>
#include <xen/xen.h>
#include <xen/grant_table.h>

#include <linux/major.h>
#include <xen/platform_pci.h>
#include <xen/interface/grant_table.h>
#include <xen/interface/io/protocols.h>
#include <asm/xen/hypervisor.h>
#include <linux/slab.h>

//Custom Includes
#include "genshm-front.h"

//#include "xenbus.c"

/************************************************************************
Interface and Util Functions
************************************************************************/




static irqreturn_t genshmf_interrupt_handler(int irq, void *dev_id){

	struct genshmf_info *info = (struct genshmf_info *)dev_id;

	//tasklet_schedule(&info->tasklet);

	return IRQ_HANDLED;
}



/*
Lots of useful stuff in pid.h/pid.c
*/
static int genshmf_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg){


	pid_t procID;
	procID = (pid_t)arg;


	#ifdef GENSHMF_DEBUG
	printk(KERN_ALERT "->genshmf_ioctl\n");
	printk(KERN_ALERT "command: %d.\n", cmd);
	#endif

	switch(cmd){
		case GENSHMF_REPORT:
			#ifdef GENSHMF_DEBUG
			printk(KERN_ALERT "Reporting\n");
			#endif
			break;
		case GENSHMF_TEST:
			#ifdef GENSHMF_DEBUG
			printk(KERN_ALERT "Testing\n");
			#endif
			/*
			struct task_struct *task;



			//Get task_struct for given pid

			for_each_process(task) {
				if ( task->pid == procID) {
					break;
				}
			}


			#ifdef GENSHMF_DEBUG
			printk(KERN_ALERT "Stopping process %d\n",procID);
			#endif

			malpage_halt_process(task);

			//Generate and store report globally
			//malpage_generate_report(task);
			//malpage_store_report(rep);

				*/
			return 0;
		break;
		default:
			#ifdef GENSHMF_DEBUG
			printk(KERN_ALERT "Command not recognized: %d\n", cmd);
			#endif
			return GENSHMF_BADCMD;
	}
	return 0;
}






//static int __init genshmf_init(void){
static int genshmf_init(void){

	struct device *err_dev;
	int result = 0;
	int end = 0;

	printk(KERN_ALERT "->genshmf_init\n");
	if (!xen_domain())
		return -ENODEV;

	/*
	if (register_blkdev(genshmf_major, DEVICE_NAME)) {
		printk(KERN_WARNING "->genshmf_init: can't get major %d with name %s\n", genshmf_major, DEVICE_NAME);
		return -ENODEV;
	}
	*/

	printk(KERN_ALERT "->genshmf_init: Loading...\n");

	//Reserve a major number
	result = alloc_chrdev_region(&genshmf_dev, GENSHMF_MIN_MINORS, GENSHMF_MAX_MINORS, DEVICE_NAME);
	genshmf_major = MAJOR(genshmf_dev);
	genshmf_minor = MINOR(genshmf_dev);


	if (genshmf_major < 0) {
		printk(KERN_ALERT "->genshmf_init: Registering the character device failed with major number: %d, minor: %d", genshmf_major,genshmf_minor);
		return -ENODEV;
	}

	//Much simpler, but required udev to run on the machine
	genshmf_class = class_create(THIS_MODULE, DEVICE_NAME);
	err_dev = device_create(genshmf_class, NULL,genshmf_dev,"%s",DEVICE_NAME);

	if (err_dev == NULL) {
		printk(KERN_ALERT "->genshmf_init: Registering the character device failed with error: %d",result);
		return -ENODEV;
	}

	#ifdef GENSHMF_DEBUG
	printk(KERN_ALERT "I was assigned major number %d\n", genshmf_major);
	printk(KERN_ALERT "PAGE_SHIFT: %d\n", PAGE_SHIFT);
	printk(KERN_ALERT "PAGE_SIZE: %lu\n", PAGE_SIZE);
	printk(KERN_ALERT "gref_list_t size: %lu\n", sizeof(gref_list_t));
	#endif

	//genshmf_xenbus_init();
	genshmf_register(genshm_front_info);

	printk(KERN_ALERT "->genshmf_init: Loaded.\n");

	return 0;

}

static void genshmf_exit(void){

	printk(KERN_ALERT "->genshmf_exit\n");
	if (xen_initial_domain())
		return;

	printk(KERN_ALERT ": genshmf_exit: Unloading...\n");

	//genshmf_xenbus_exit();

	//Unregister the device
	device_destroy(genshmf_class,genshmf_dev);
	class_destroy(genshmf_class);
	unregister_chrdev(genshmf_major, DEVICE_NAME);

	printk(KERN_ALERT "genshmf_exit: Unloaded.\n");

}


static int genshmf_register(struct genshmf_info* info){

	int result;
	char *domid_str;
	char *value;
	struct xenbus_transaction *xstrans;
	int ret;

	#ifdef GENSHMF_DEBUG
	printk(KERN_ALERT "->genshmf_register: building ring share.\n");
	#endif

	info = kzalloc(sizeof(struct genshmf_info),GFP_KERNEL);
	info->ring_mfn = genshmf_setup_ring(info);

	#ifdef GENSHMF_DEBUG
	printk(KERN_ALERT "->genshmf_register: done building ring share\n");
	printk(KERN_ALERT "->genshmf_register: beginning creation of xs value\n");
	#endif

	info->uuid = kmalloc(GENSHMF_UUID_LENGTH,0);
	genshmf_get_uuid(info->uuid);
	info->domid = genshmf_get_domid();

	//Ugly hack, but why not?
	value = kmalloc(strlen(GENSHMF_XENSTORE_REGISTER_VALUE_FORMAT)+GENSHMF_UUID_LENGTH+15,0);
	if((ret = sprintf(value, GENSHMF_XENSTORE_REGISTER_VALUE_FORMAT, info->domid, info->gref , info->evtchn, info->uuid)) < 1){
		#ifdef GENSHMF_DEBUG
		printk(KERN_ALERT "->genshmf_register: sprintf broke: %d\n",ret);
		#endif
		return GENSHMF_GENERALERR;
	}

	#ifdef GENSHMF_DEBUG
	printk(KERN_ALERT "->genshmf_register: done creation of xs value with %s\n",value);
	printk(KERN_ALERT "->genshmf_register: beginning registration\n");
	#endif

	xstrans = kmalloc(sizeof(struct xenbus_transaction),0);
	result = xenbus_transaction_start(xstrans);

	//Get a string version of the domid to use in the path
	domid_str = kmalloc(strlen("10000"),0);
	sprintf(domid_str, "%u", info->domid);

	/*
	The connection from the domU has an implicit root at /local/domain/<domid>.
	If you specify a path without a leading /, then the implicit root is used, so
	you can see your own vm path simply by reading "vm", your own domain ID by
	reading "domid", etc.  However, domU's cannot by default read what is stored
	inside /vm, even their own, as a security precaution.
	*/
	ret = xenbus_write(*xstrans, GENSHMF_XENSTORE_REGISTER_PATH, domid_str, value);

	/*
	if (IS_ERR(tmp)){
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "error: %li.\n",PTR_ERR(tmp));
		#endif
		return MALPAGE_XSERR;
	}
	*/

	//Finish up
	result = xenbus_transaction_end(*xstrans, 0);

	//Clean up
	kfree(value);
	kfree(xstrans);

	#ifdef GENSHMF_DEBUG
	printk(KERN_ALERT "->genshmf_register: done registration\n");
	#endif

	return 0;

}




//////////////////////////////////////////////////////////////////////////////
/**
 * Entry point to this code when a new device is created.  Allocate the basic
 * structures and switch to InitWait.
 */
static int genshmf_probe(struct xenbus_device* dev, const struct xenbus_device_id* id)
{
        printk(KERN_ALERT "->genshmf_probe\n");

        genshm_front_info = kzalloc(sizeof(genshm_front_info),GFP_KERNEL);
        genshm_front_info->xbdev = dev;

        //genshmf_init_ring(dev,genshm_front_info);

        //printk(KERN_ALERT "->Frontend State: XenbusStateInitialised\n");
        //xenbus_switch_state(dev, XenbusStateInitialised);
        xenbus_switch_state(dev, XenbusStateConnected);
        return 0;
}

/**
 * Callback received when the backend's state changes.
 */
static void genshmb_changed(struct xenbus_device *dev, enum xenbus_state backend_state)
{
	//struct genshmb_info *info = dev_get_drvdata(&dev->dev);

	printk(KERN_ALERT "->genshmb_changed: backend changed to state %s.\n", xenbus_strstate(backend_state));

	/*
	XenbusStateUnknown      = 0,
	xenbusStateInitialising = 1,
	XenbusStateInitWait     = 2,  // Finished early initialisation, but waiting for information from the peer or hotplug scripts.
	XenbusStateInitialised  = 3,  // Initialised and waiting for a connection from the peer.
	XenbusStateConnected    = 4,
	XenbusStateClosing      = 5,  // The device is being closed due to an error or an unplug event.
	XenbusStateClosed       = 6
	*/

	switch (backend_state) {
		case XenbusStateInitialising:
		case XenbusStateInitWait:
		case XenbusStateInitialised:
		case XenbusStateReconfiguring:
		case XenbusStateReconfigured:
		case XenbusStateUnknown:
		case XenbusStateClosed:
		case XenbusStateConnected:
			xenbus_switch_state(dev, XenbusStateConnected);
			printk(KERN_ALERT "->genshmf: Ready.\n", xenbus_strstate(backend_state));
			break;
		case XenbusStateClosing:
			xenbus_switch_state(dev, XenbusStateClosing);
			break;
	}
}


static int genshmf_setup_ring(struct genshmf_info *info){

	struct genshm_sring *sring;
	int err;

	printk(KERN_ALERT "->genshmf_setup_ring\n");

	info->gref = GENSHM_GRANT_INVALID_REF;

	sring = (struct genshm_sring*)__get_free_page(GFP_NOIO | __GFP_HIGH);
	if (!sring) {
		printk(KERN_ALERT "->genshmf_setup_ring: Error allocating ring");
		//xenbus_dev_fatal(dev, -ENOMEM, "allocating shared ring");
		return -ENOMEM;
	}

	SHARED_RING_INIT(sring);
	FRONT_RING_INIT(&info->fring, sring, PAGE_SIZE);

	//sg_init_table(info->sg, BLKIF_MAX_SEGMENTS_PER_REQUEST);

	err = gnttab_grant_foreign_access(GENSHMF_DOM0_ID, virt_to_mfn(info->fring.sring), 0);
	//err = xenbus_grant_ring(dev, virt_to_mfn(info->fring.sring));
	if (err < 0) {
		free_page((unsigned long)sring);
		info->fring.sring = NULL;
		goto fail;
	}
	info->gref = err;

	//err = xenbus_alloc_evtchn(dev, &info->evtchn);
	err = genshmf_alloc_evtchn(info->domid, &info->evtchn); //Wrote my own
	if (err)
		goto fail;

	err = bind_evtchn_to_irqhandler(info->evtchn, genshmf_interrupt_handler, 0, GENSHMF_CHANNEL_NAME, info);
	if (err <= 0) {
		//xenbus_dev_fatal(dev, err, "");
		printk(KERN_ALERT "->genshmf_setup_ring: bind_evtchn_to_irqhandler failed");
		goto fail;
	}
	info->irq = err;

	return 0;
fail:
	genshmf_free_ring(info);
	return err;
}


static void genshmf_free_ring(struct genshmf_info *info){

	if (info->irq)
		unbind_from_irqhandler(info->irq, info);

	info->evtchn = info->irq = 0;

}

static int genshmf_alloc_evtchn(int domid, int *port){

         struct evtchn_alloc_unbound alloc_unbound;
         int err;

         alloc_unbound.dom = DOMID_SELF;
         alloc_unbound.remote_dom = domid;

         err = HYPERVISOR_event_channel_op(EVTCHNOP_alloc_unbound, &alloc_unbound);
         if (err)
				 printk(KERN_ALERT "->genshmf_alloc_evtchn: Error allocating event channel");
         else
                 *port = alloc_unbound.port;

         return err;
}


static int genshmf_free_evtchn(int port){

	        struct evtchn_close close;
	        int err;

        close.port = port;

        err = HYPERVISOR_event_channel_op(EVTCHNOP_close, &close);
        if (err)
				printk(KERN_ALERT "->genshmf_free_evtchn: Error freeing event channel %d", port);

        return err;
}

static int genshmf_init_ring(struct xenbus_device *dev, struct genshmf_info *info){

	const char *message = NULL;
	struct xenbus_transaction xbt;
	int err;

	//Create shared ring, alloc event channel.
	//err = genshmf_setup_ring(dev, info);

	if (err)
		goto out;

again:
	err = xenbus_transaction_start(&xbt);
	if (err) {
		xenbus_dev_fatal(dev, err, "starting transaction");
		goto destroy_genshmring;
	}

	err = xenbus_printf(xbt, dev->nodename,"ring-ref", "%u", info->gref);
	if (err) {
		message = "writing ring-ref";
		goto abort_transaction;
	}
	err = xenbus_printf(xbt, dev->nodename,"event-channel", "%u", info->evtchn);
	if (err) {
		message = "writing event-channel";
		goto abort_transaction;
	}

	err = xenbus_transaction_end(xbt, 0);
	if (err) {
		if (err == -EAGAIN)
			goto again;
		xenbus_dev_fatal(dev, err, "completing transaction");
		goto destroy_genshmring;
	}

	return 0;

 abort_transaction:
	xenbus_transaction_end(xbt, 1);
	if (message)
		xenbus_dev_fatal(dev, err, "%s", message);
 destroy_genshmring:
	 genshmf_free_ring(info);
 out:
	return err;
}



static int genshmf_get_uuid(char* uuid){

	struct file* fd;
	mm_segment_t old_fs;
	unsigned long long offset;

	offset = 0;
	old_fs = get_fs();

	set_fs(get_ds()); //Critical otherwise the buffer will be in userspace
	fd = filp_open(GENSHMF_UUID_LOC, O_RDONLY, 0);

	if(IS_ERR(fd)) {
		#ifdef GENSHMF_DEBUG
		printk(KERN_ALERT "->genshmf_get_uuid: error opening: %li.\n",PTR_ERR(fd));
		#endif
		return GENSHMF_SYSFSERR;
    }

	if( vfs_read(fd, uuid, GENSHMF_UUID_LENGTH, &offset ) != GENSHMF_UUID_LENGTH){
			#ifdef GENSHMF_DEBUG
			printk(KERN_ALERT "->genshmf_get_uuid problem reading sysfs.\n");
			#endif
			return GENSHMF_SYSFSERR;
	}

	set_fs(old_fs);
	filp_close(fd, NULL);
	uuid[GENSHMF_UUID_LENGTH-1] = '\0'; //need to null terminate that bad boy and also remove newline

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "->genshmf_get_uuid: got %s\n",uuid);
	#endif

	return 0;

}

static int genshmf_get_domid(void){

	int result;
	int len;
	void *tmp;
	char *tmp2;
	int value;
	struct xenbus_transaction *xstrans;

	#ifdef GENSHMF_DEBUG
	printk(KERN_ALERT "->genshmf_get_domid: getting domid.\n");
	#endif


	xstrans = kmalloc(sizeof(struct xenbus_transaction),0);
	result = xenbus_transaction_start(xstrans);

	/*
	The connection from the domU has an implicit root at /local/domain/<domid>.
	If you specify a path without a leading /, then the implicit root is used, so
	you can see your own vm path simply by reading "vm", your own domain ID by
	reading "domid", etc.  However, domU's cannot by default read what is stored
	inside /vm, even their own, as a security precaution.
	*/
	tmp = xenbus_read(*xstrans, GENSHMF_XENSTORE_DOMID_PATH, "", &len);

	if (IS_ERR(tmp)){
		#ifdef GENSHMF_DEBUG
		printk(KERN_ALERT "->genshmf_get_domid: error: %li.\n",PTR_ERR(tmp));
		#endif
		return GENSHMF_XSERR;
	}

	#ifdef GENSHMF_DEBUG
	printk(KERN_ALERT "->genshmf_get_domid: query successful\n");
	#endif

	tmp2 = kmalloc(len,0);
	strncpy(tmp2,(char*)tmp,len);
	value = simple_strtoul(tmp2,NULL,10);

	//Finish up
	result = xenbus_transaction_end(*xstrans, 0);
	kfree(xstrans);

	#ifdef GENSHMF_DEBUG
	printk(KERN_ALERT "->genshmf_get_domid: got domid %d\n",value);
	#endif

	return value;

}


//////////////////////////////////////////////////////////////////////////////
void genshmf_xenbus_init(){
	xenbus_register_frontend(&genshmf_driver);
}

void genshmf_xenbus_exit(){
	//genshmf_free_ring(genshm_front_info);
	xenbus_unregister_driver(&genshmf_driver);
}

module_init(genshmf_init);
module_exit(genshmf_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("CHRISBENNINGER");
MODULE_ALIAS("genshmf");
