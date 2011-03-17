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
#include "monitor.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("CHRISBENNINGER");


/************************************************************************

Interface and Util Functions

************************************************************************/

static int monitor_init(void) {

	int result = 0;
	struct device *err_dev;

	printk(KERN_ALERT "->monitor_init: Loading...\n");

	//Reserve a major number
	result = alloc_chrdev_region(&monitor_dev, MONITOR_MIN_MINORS, MONITOR_MAX_MINORS, DEVICE_NAME);
	monitor_major = MAJOR(monitor_dev);
	monitor_minor = MINOR(monitor_dev);
	
	if (monitor_major < 0) {
		printk(KERN_ALERT "->monitor_init: Registering the character device failed with major number: %d, minor: %d", monitor_major,monitor_minor);
		return -ENODEV;
	}

	//Much simpler, but required udev to run on the machine
	monitor_class = class_create(THIS_MODULE, DEVICE_NAME);

    /* Connect the file operations with the cdev */
	cdev_init(&monitor_cdev, &monitor_fops);
	monitor_cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	if (cdev_add(&monitor_cdev, monitor_dev, 1)) {
		printk(KERN_ALERT "->monitor_init: Failed with registering the character device");
		return 1;
	}

	err_dev = device_create(monitor_class, NULL,monitor_dev,"%s",DEVICE_NAME);
	if (err_dev == NULL) {
		printk(KERN_ALERT "->monitor_init: Registering the character device failed with error: %d",result);
		return -ENODEV;
	}
	
	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "I was assigned major number %d\n", monitor_major);
	printk(KERN_ALERT "PAGE_SHIFT: %d\n", PAGE_SHIFT);
	printk(KERN_ALERT "PAGE_SIZE: %lu\n", PAGE_SIZE);
	#endif
		
	printk(KERN_ALERT "->monitor_init: Loaded.\n");
	
	//Initialize Grant Table
	/*
	result = gnttab_init();
	if(result){
		printk(KERN_ALERT "->monitor_init:  failed initializing grant table: %d\n",result);
		//return MALPAGE_GENERALERR;
	}
	 */

	return 0;
}




static void monitor_exit(void) {
	printk(KERN_ALERT "Unloading...\n");
	
	/* Unregister the device */
	//cleanup_grant();
	device_destroy(monitor_class,monitor_dev);
	cdev_del(&monitor_cdev);
	class_destroy(monitor_class);
	//unregister_chrdev(monitor_major, DEVICE_NAME);
	

	printk(KERN_ALERT "Unloaded.\n");
}




static int monitor_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg) {
	
	process_report_t* rep;

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "monitor_ioctl\n");
	printk(KERN_ALERT "command: %d.\n", cmd);
	#endif
	


	switch(cmd){
		case MONITOR_REGISTER:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Registering domain.\n");
			#endif
			monitor_share_info = monitor_populate_info(arg);
			monitor_register(monitor_share_info);
			return 0;
		break;
		case MONITOR_REPORT:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Received report\n");
			#endif
			rep = monitor_populate_report(arg);
			monitor_report(rep);
			return 0;
		break;
		case MONITOR_DEREGISTER:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Deregistering domain.\n");
			#endif
			//monitor_register((( monitor_uspace_info_t*)arg));
			return 0;
			
		break;
		default:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Command not recognized: %d\n", cmd);
			#endif
			return MONITOR_BADCMD;	
			
	}

	return MONITOR_SUCCESS;

}




/************************************************************************

Grant table and Interdomain Functions

************************************************************************/


static monitor_share_info_t* monitor_populate_info(unsigned long arg){

	monitor_uspace_info_t* tmp_info;
	monitor_share_info_t *info;

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "->monitor_populate_info");
	#endif

	tmp_info = (monitor_uspace_info_t*)arg;
	info = kzalloc(sizeof(monitor_share_info_t),GFP_KERNEL);

	//info->uuid = kmalloc(GENSHMB_UUID_LENGTH,0);
	//info->uuid = tmp_info->uuid;

	info->domid = tmp_info->domid;
	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "->monitor_populate_info: domid: %u",info->domid);
	#endif

	info->gref = tmp_info->gref;
	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "->monitor_populate_info: gref: %u",info->gref);
	#endif

	info->evtchn = tmp_info->evtchn;
	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "->monitor_populate_info: evtchn: %u",info->evtchn);
	#endif

	return info;
}


static int monitor_register(monitor_share_info_t *info){

	int err;
	struct vm_struct *v_start;
	struct gnttab_map_grant_ref ops;
	struct gnttab_unmap_grant_ref unmap_ops;
	//struct gnttab_setup_table setup_ops;

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "->monitor_register: got gref:%u domid:%u\n",info->gref,info->domid);
	#endif
	
	if(!info){
		printk(KERN_ALERT "Info struct not properly initialized\n");
		return MONITOR_ALLOCERR;
	}

	/*
	setup_ops.dom = info->domid;
	setup_ops.nr_frames = MONITOR_GNTTAB_SIZE;

	if (HYPERVISOR_grant_table_op(GNTTABOP_setup_table, &setup_ops, 1)) {
		printk(KERN_ALERT "monitor_register: HYPERVISOR GNTTABOP_setup_table failed\n");
		return -EFAULT;
	}
	if (setup_ops.status) {

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

		printk(KERN_ALERT "->monitor_register:  HYPERVISOR GNTTABOP_setup_table status = %d\n", setup_ops.status);
		return -EFAULT;
	}
*/


	// The following function reserves a range of kernel address space and
	// allocates pagetables to map that range. No actual mappings are created.
	v_start = alloc_vm_area(PAGE_SIZE);
	if (v_start == 0) {
		free_vm_area(v_start);
		printk(KERN_ALERT "->monitor_register: could not allocate page\n");
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
		printk(KERN_ALERT "monitor_register: HYPERVISOR map grant ref failed\n");
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
		printk(KERN_ALERT "->monitor_register:  HYPERVISOR map grant ref failed status = %d\n", ops.status);
		return -EFAULT;
		
	}

	unmap_ops.host_addr = (unsigned long)(v_start->addr);
	unmap_ops.handle = ops.handle;

	//Get a handle on the ring sitting in the page
	sring = (struct as_sring*)v_start->addr;
	BACK_RING_INIT(&(info->bring), sring, PAGE_SIZE);

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "->monitor_register: Back ring initialized\n");
	#endif

	//Setup an event channel to the frontend 
	err = bind_interdomain_evtchn_to_irqhandler(info->domid, info->evtchn, monitor_irq_handle, 0, MONITOR_CHANNEL_NAME, info);
	
	if (err < 0) {
		#ifdef MONITOR_DEBUG
		printk(KERN_ALERT "->monitor_register: failed binding to evtchn with err: %d\n",err);
		#endif
		err = HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, &unmap_ops, 1);
		return -EFAULT;
	}
	
	//info->irq = err;
	info->evtchn = err;

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "->monitor_register: done\n");
	#endif
	
	return 0;
}






static irqreturn_t monitor_irq_handle(int irq, void *dev_id){

		RING_IDX rc, rp;
		struct request_t req;
		struct response_t resp;
		int notify;
		monitor_share_info_t *monitor_share_info;
		
		#ifdef MONITOR_DEBUG
		printk(KERN_ALERT "Dom0: Handling Event\n");
		#endif

		monitor_share_info = (monitor_share_info_t*)dev_id;

		rc = monitor_share_info->bring.req_cons;
		rp = monitor_share_info->bring.sring->req_prod;
		
		//Fetch Each Request
		while(rc!=rp) {
		
			//If we are out of things to process, exit loop
			if(RING_REQUEST_CONS_OVERFLOW(&monitor_share_info->bring, rc))
				break;

			resp.operation = MONITOR_RING_NONOP;

			//Get a copy of the request
			memcpy(&req, RING_GET_REQUEST(&monitor_share_info->bring, rc), sizeof(req));

			printk(KERN_ALERT "Report summary: procid: %u, domid: %u, age: %u, numpfns: %u\n",req.report.process_id, req.report.domid, req.report.process_age, req.report.pfn_list_length);

			// update the req-consumer
			monitor_share_info->bring.req_cons = ++rc;
			barrier();

			switch (req.operation) {
			
				case MONITOR_RING_REPORT :
				
					printk(KERN_ALERT "\nMonitor, Got MONITOR_RING_REPORT op: %u", req.operation);

					//resp.operation = MONITOR_RING_QUERY_PFNLIST;
					resp.operation = monitor_report(&(req.report));
					//more_to_do = 1;
					resp.report = req.report;
					//return IRQ_HANDLED;
					break;

				default:
					printk(KERN_ALERT "\nMonitor, Unrecognized operation: %u", req.operation);
					break;
					  
			}

			memcpy(RING_GET_RESPONSE(&monitor_share_info->bring, monitor_share_info->bring.rsp_prod_pvt), &resp, sizeof(resp));
			monitor_share_info->bring.rsp_prod_pvt++;

			RING_PUSH_RESPONSES_AND_CHECK_NOTIFY(&monitor_share_info->bring, notify);
			
			printk(KERN_ALERT "\nMonitor, Sending response\n");
			//notify_remote_via_irq(monitor_share_info->irq);
			notify_remote_via_irq(monitor_share_info->evtchn);

			/*if(monitor_share_info->bring.rsp_prod_pvt == monitor_share_info->bring.req_cons) {
				  RING_FINAL_CHECK_FOR_REQUESTS(&monitor_share_info->bring, more_to_do);
			} else if (RING_HAS_UNCONSUMED_REQUESTS(&monitor_share_info->bring)) {
				  more_to_do = 1;
			}
			if(notify) {
				  printk("\nxen:Dom0: Send notify to DomU");
				  notify_remote_via_irq(monitor_share_info->irq);
			}
			*/
		}

		return IRQ_HANDLED;
}



static int monitor_report(process_report_t *rep) {

	//struct vm_struct** vm_struct_list[rep->pfn_list_length];
	int i;
	int j;
	struct vm_struct* tmp_vm_struct;

	#ifdef MONITOR_DEBUG

	printk(KERN_ALERT "Dom0: Report Received:\n");
	printk(KERN_ALERT "	process_id: %u\n",rep->process_id);
	printk(KERN_ALERT "	domid: %u\n",rep->domid);
	printk(KERN_ALERT "	pfn_list_length: %u\n",rep->pfn_list_length);
	printk(KERN_ALERT "	pfn_list:	");

	for(i=0; i < rep->pfn_list_length; i++){
		printk(KERN_ALERT "%lu",rep->pfn_list[i]);
	}

	#endif

	vm_struct_list = kzalloc(rep->pfn_list_length*PAGE_SIZE,0);
	//vm_area_list = kzalloc(rep->pfn_list_length*(sizeof(unsigned int*)),0);
	j = 0;
	for(i = 0; i < rep->pfn_list_length; i++){
		tmp_vm_struct = monitor_map_gref(rep->gref_list[i],rep->domid);
		if(tmp_vm_struct->size > 0){
			vm_struct_list[j] = tmp_vm_struct;
			j++;
		}
	}

	vm_struct_list_size = j;
	//vm_area_list_size = j;
	//vm_struct_list_size = rep->pfn_list_length;

	printk(KERN_ALERT "Successfully mapped %d pages",vm_struct_list_size);
	/*
	for(i = 0; i < j; i++){
		printk(KERN_ALERT "Size: index %d, %lu",i,vm_struct_list[i]->size);
	}*/

	//Do analysis

	//Unmap RANGE

	//Kill process
	//return MONITOR_RING_KILL;

	//Dont Kill process
	return MONITOR_RING_RESUME;


}


static unsigned long monitor_unmap_range(unsigned long addr_start, int length, int blocksize){


}


static process_report_t* monitor_populate_report(unsigned long arg){

	process_report_t *tmp_rep;
	process_report_t *rep;
	int failed = 0;

	tmp_rep = (process_report_t*)arg;
	rep = kzalloc(sizeof(process_report_t),GFP_KERNEL);
	
	rep->domid = tmp_rep->domid;
	rep->process_id = tmp_rep->process_id;
	rep->process_age = tmp_rep->process_age;
	rep->pfn_list_length = tmp_rep->pfn_list_length;

	printk(KERN_ALERT "domid %u, pid %u, age %u, count %u",rep->domid,rep->process_id, rep->process_age, rep->pfn_list_length);

	rep->pfn_list = kzalloc((rep->pfn_list_length)*sizeof(unsigned long),GFP_KERNEL);
	failed = copy_from_user((void*)rep->pfn_list,(void*)tmp_rep->pfn_list,(rep->pfn_list_length)*sizeof(unsigned long));

	if(failed>0){
		printk(KERN_ALERT "Unable to copy %u bytes of pfn list from userspace",failed);
	}
	else if(failed<0){
		printk(KERN_ALERT "Err: %d",failed);
	}

	rep->gref_list = kzalloc((rep->pfn_list_length)*sizeof(unsigned int),GFP_KERNEL);
	failed = copy_from_user((void*)rep->gref_list,(void*)tmp_rep->gref_list,(rep->pfn_list_length)*sizeof(unsigned int));
	if(failed>0){
		printk(KERN_ALERT "Unable to copy %u bytes of gref list from userspace",failed);
	}
	else if(failed<0){
		printk(KERN_ALERT "Err: %d",failed);
	}

	return rep;
	/*
	printk(KERN_EMERG "1\n");
	v_start = alloc_vm_area(PAGE_SIZE);	 // Get a vmarea for a page. No actual mappings are created.
	printk(KERN_EMERG "2\n");
	if (v_start == 0) {
		free_vm_area(v_start);
		printk(KERN_ALERT "populate_report: could not allocate page\n");
		return -EFAULT;
	}
	printk(KERN_EMERG "3\n");
	gnttab_set_map_op(&ops, (phys_addr_t)((v_start->addr)+(PAGE_SIZE)), GNTMAP_host_map, *next_gref, (rep->domid));
	printk(KERN_EMERG "4\n");
	if (HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, &ops, 1)) {
		printk(KERN_ALERT "populate_report: HYPERVISOR map grant ref failed\n");
		return -EFAULT;
	}
	printk(KERN_EMERG "5\n");
	if (ops.status) {
		printk(KERN_ALERT "populate_report:  HYPERVISOR map grant ref failed status = %d\n", ops.status);
		return -EFAULT;
	}
	
	int firstInt = 0;
	int lastInt = 0;
	
	printk(KERN_EMERG "6\n");
	memcpy(&firstInt,v_start->addr,sizeof(int));
	printk(KERN_EMERG "7\n");
	memcpy(&lastInt,v_start->addr+(PAGE_SIZE-sizeof(int)),sizeof(int));
	printk(KERN_EMERG "8\n");
	
	printk(KERN_EMERG "%d, %d\n",firstInt,lastInt);
	*/
	

}

//Borrowed this code and tweaked from the blkback driver
/*
static inline unsigned long vaddr(unsigned long *addr){
	unsigned long pfn = page_to_pfn(virt_to_page(addr));
	return (unsigned long)pfn_to_kaddr(pfn);
}*/



/*
static unsigned long monitor_map_pageblock(process_report_t *rep){

	struct vm_struct** v_start; //processes vm area
	struct gnttab_map_grant_ref ops[rep->pfn_list_length];
	int i;

	// Get a vmarea large enough to hold processes pages. No actual mappings are created.
	v_start = alloc_vm_area((size_t)(PAGE_SIZE*(rep->pfn_list_length)));

	if (v_start == 0) {
		free_vm_area(v_start);
		printk(KERN_ALERT "monitor_map_pageblock: could not allocate page\n");
		return -EFAULT;
	}

	//Map in the remote pages one by one
	for (i=0; i < rep->pfn_list_length ; i++){
		if(rep->gref_list[i]<0){
			printk(KERN_ALERT "monitor_map_pageblock: gref is <0. Aborting\n");
			return -EFAULT;
		}

		printk(KERN_ALERT "monitor_map_pageblock: HYPERVISOR mapping gref: %d\n",rep->gref_list[i]);
		gnttab_set_map_op(&ops[i], (((unsigned long)(v_start->phys_addr))+(i*PAGE_SIZE)), GNTMAP_host_map|GNTMAP_readonly, rep->gref_list[i], rep->domid);
		printk(KERN_ALERT "map_process: mapped pfn %lu\n",rep->pfn_list[i]);

		if (HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, &ops[i], rep->pfn_list_length)) {
			printk(KERN_ALERT "monitor_map_pageblock: HYPERVISOR map grant ref failed\n");

			return -EFAULT;
		}
		if (ops[i].status) {
			printk(KERN_ALERT "monitor_map_pageblock:  HYPERVISOR map grant ref failed status = %d\n", ops[i].status);
			printk(KERN_ALERT "monitor_map_pageblock:  ERR%d\n", ERR_PTR(ops[i].status));
			return -EFAULT;
		}

	}

	printk(KERN_ALERT "monitor_map_pageblock\n");
	return (unsigned long)v_start->addr;
}*/
 

static struct vm_struct* monitor_map_gref(unsigned int gref, unsigned int domid){

	struct vm_struct* v_start; //processes vm area
	struct gnttab_map_grant_ref ops;

	// Get a vmarea large enough to hold processes pages. No actual mappings are created.
	v_start = alloc_vm_area((size_t)(PAGE_SIZE));

	if (v_start == 0) {
		free_vm_area(v_start);
		printk(KERN_ALERT "monitor_map_gref: could not allocate page\n");
	}

	if(gref<1){
		printk(KERN_ALERT "monitor_map_gref: gref is <0. Aborting\n");
		v_start->size = 0;
	}

	printk(KERN_ALERT "monitor_map_gref: HYPERVISOR mapping gref: %d\n",gref);



	gnttab_set_map_op(&ops, ((unsigned long)(v_start->addr)), GNTMAP_host_map|GNTMAP_readonly, (grant_ref_t)gref, (domid_t)domid );
	//GNTMAP_host_map|GNTMAP_application_map|GNTMAP_contains_pte|GNTMAP_readonly
/*
	ops.flags = GNTMAP_host_map;
	ops.ref = gref;
	ops.dom = domid;
	ops.host_addr = (unsigned long)(v_start->addr);
*/
	if (HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, &ops, 1)) {
		printk(KERN_ALERT "%s: HYPERVISOR map grant ref failed\n",__FUNCTION__);
		v_start->size = 0;
	}
	if (ops.status){
		printk(KERN_ALERT "monitor_map_gref:  HYPERVISOR map grant ref failed status = %d\n", ops.status);
		//free_vm_area(v_start);
		v_start->size = 0;
	}
	else{
		printk(KERN_ALERT "monitor_map_gref: test: %d",*((int*)v_start->addr));
		printk("\nmonitor_map_gref: shared_page = %x, handle = %x, status = %x",(unsigned int)v_start->addr, ops.handle, ops.status);
	}



	return v_start;

}

/* */
/*static int monitor_map( monitor_share_info_t info) {*/

/*      struct vm_struct *v_start;*/
/*      as_sring_t *sring;*/
/*      int err;*/
/*      */
/* 	  gref = 932;*/
/*      info.gref = gref;*/
/*      info.remoteDomain = 3;*/
/*      info.evtchn = port;*/
/*      */
/*      printk(KERN_ALERT "\nxen: dom0: init_module with gref = %d", info.gref);*/
/* */
/*      // The following function reserves a range of kernel address space and*/
/*      // allocates pagetables to map that range. No actual mappings are created.*/
/*      v_start = alloc_vm_area(PAGE_SIZE);*/
/*      if (v_start == 0) {*/
/*            free_vm_area(v_start);*/
/*            printk(KERN_ALERT "\nxen: dom0: could not allocate page");*/
/*            return -EFAULT;*/
/*      }*/
/*      */
/*         ops struct in paramaeres*/
/*            host_addr, flags, ref*/
/*         ops struct out parameters*/
/*            status (zero if OK), handle (used to unmap later), dev_bus_addr*/
/*      */
/*      gnttab_set_map_op(&ops, (unsigned long)v_start->addr, GNTMAP_host_map, info.gref, info.remoteDomain); // flags, ref, domID */
/* */
/*      if (HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, &ops, 1)) {*/
/*            printk("\nxen: dom0: HYPERVISOR map grant ref failed");*/
/*            return -EFAULT;*/
/*      }*/
/*      if (ops.status) {*/
/*            printk("\nxen: dom0: HYPERVISOR map grant ref failed status = %d", ops.status);*/
/*            return -EFAULT;*/
/*      }*/
/*     // printk(KERN_ALERT "\nxen: dom0: shared_page = %u, handle = %x, status = %x", (unsigned int)v_start->addr, ops.handle, ops.status);*/
/*      // Used for unmapping*/
/*      unmap_ops.host_addr = (unsigned long)(v_start->addr);*/
/*      unmap_ops.handle = ops.handle;*/
/*    */
/*      */
/*	  int i;*/
/*      printk("\nBytes in page ");*/
/*      for(i=0;i<=10;i++) {*/
/*            printk("%c", ((char*)(v_start->addr))[i]);*/
/*      }*/
/*	*/

/*      sring = (as_sring_t*)v_start->addr;*/
/*      BACK_RING_INIT(&info.ring, sring, PAGE_SIZE);*/
/* */
/*      //Seetup an event channel to the frontend */
/*      err = bind_interdomain_evtchn_to_irqhandler(info.remoteDomain, info.evtchn, monitor_irq_handle, 0, "dom0-backend", &info);*/
/*        if (err < 0) {*/
/*            printk("\nxen: dom0: init_module failed binding to evtchn !");*/
/*            err = HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, &unmap_ops, 1);*/
/*            return -EFAULT;*/
/*      }*/
/*      info.irq = err;*/
/*     */
/*      printk("\nxen: dom0: end init_module: int = %d", info.irq);*/
/*      return 0;*/
/*}*/
/* */
static void cleanup_grant(void) {
      //int ret;
 /*
      printk("\nxen: dom0: cleanup_module");
      // Unmap foreign frames
      // ops.handle points to the pages that were initially mapped. Set in the
      // __init() function
      //ops.host_addr ponts to the heap where the pages were mapped
      ret = HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, &unmap_ops, 1);
      if (ret == 0) {
            printk(" cleanup_module: unmapped shared frame");
      } else {
            printk(" cleanup_module: unmapped shared frame failed");
      }
      */
}

/*
static void monitor_dump_pages(unsigned long *addr, unsigned int numpages){

	unsigned int j;
	unsigned int i;

	for( j = 0; j<MONITOR_DUMP_COUNT && j<numpages; j++){

		//page = mfn_to_virt(mfnlist[j]);

		for( i = 0; i < PAGE_SIZE; i++){
			//printk(KERN_ALERT "%x",*(addr+i));
		}
	}

	return;
}*/


static ssize_t monitor_read(struct file *filp, char *buffer, size_t count, loff_t *offp){

	int byte_count;
	int i;
	int forward;
	int leftover;
	unsigned long throwaway;

	byte_count = 0;
	forward = 1;
	leftover = count;
	throwaway = 0;
	i = 0;

	//printk(KERN_ALERT "count %d\n",(int)count);

	//If the offset is more than we have data, just return
	if(*offp > vm_struct_list_size*MONITOR_VMSTRUCT_SIZE){
		return 0;
	}

	//find index to start at;
	while(i*MONITOR_VMSTRUCT_SIZE<*offp){
		//printk(KERN_ALERT "looking forward\n");
		i++;
	}

	if(leftover > (vm_struct_list_size-i)*MONITOR_VMSTRUCT_SIZE){
		leftover = (vm_struct_list_size-i)*MONITOR_VMSTRUCT_SIZE;
	}

	//printk(KERN_ALERT "2\n");

	printk(KERN_ALERT "leftover %d, offset: %d, count: %d\n",(int)leftover, (int)*offp, (int)count);
	while(leftover>0){
		printk(KERN_ALERT "leftover %d, offset: %d, count: %d\n",(int)leftover, (int)*offp, (int)count);
		//printk(KERN_ALERT "2.1\n");
		throwaway = 0;
		//Copy entire block
		if(leftover>=MONITOR_VMSTRUCT_SIZE){
			printk(KERN_ALERT "3\n");
			throwaway = copy_to_user((buffer+byte_count),vm_struct_list[i]->addr,MONITOR_VMSTRUCT_SIZE);
			printk(KERN_ALERT "failed copying %lu bytes\n",throwaway);
			i++;
			leftover = leftover - MONITOR_VMSTRUCT_SIZE;
			byte_count = byte_count + (MONITOR_VMSTRUCT_SIZE-throwaway);
		}
		//Copy segment of a block and return
		else{
		//if(leftover<MONITOR_VMSTRUCT_SIZE){
			printk(KERN_ALERT "4\n");
			throwaway = copy_to_user((buffer+byte_count),vm_struct_list[i]->addr,leftover);
			printk(KERN_ALERT "failed copying %lu bytes\n",throwaway);
			i++;
			byte_count = byte_count + (leftover-throwaway) ;
			leftover=0;
			break;
		}

	}
	//printk(KERN_ALERT "5\n");
	return byte_count;

}




module_init( monitor_init);
module_exit( monitor_exit);

