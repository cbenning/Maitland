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
#include <xen/interface/io/blkif.h>
#include <xen/interface/xen.h>
#include <xen/interface/elfnote.h>
#include <xen/xenbus.h>
#include <xen/events.h>
#include <config/sys/hypervisor.h>
#include <asm/hypervisor.h>


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

	printk(KERN_ALERT "Loading...\n");

	//Reserve a major number
	result = alloc_chrdev_region(&monitor_dev, MONITOR_MIN_MINORS, MONITOR_MAX_MINORS, DEVICE_NAME);
	monitor_major = MAJOR(monitor_dev);
	monitor_minor = MINOR(monitor_dev);
	
	if (monitor_major < 0) {
		printk(KERN_ALERT "Registering the character device failed with major number: %d, minor: %d", monitor_major,monitor_minor);
		return -ENODEV;
	}

	//Much simpler, but required udev to run on the machine
	monitor_class = class_create(THIS_MODULE, DEVICE_NAME);
	err_dev = device_create(monitor_class, NULL,monitor_dev,"%s",DEVICE_NAME);

	if (err_dev == NULL) {
		printk(KERN_ALERT "Registering the character device failed with error: %d",result);
		return -ENODEV;
	}

	
	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "I was assigned major number %d\n", monitor_major);
	printk(KERN_ALERT "PAGE_SHIFT: %d\n", PAGE_SHIFT);
	printk(KERN_ALERT "PAGE_SIZE: %lu\n", PAGE_SIZE);
	#endif
		
	printk(KERN_ALERT "Loaded.\n");

	
	return 0;
}




static void monitor_exit(void) {
	printk(KERN_ALERT "Unloading...\n");
	
	/* Unregister the device */
	cleanup_grant();
	device_destroy(monitor_class,monitor_dev);
	//cdev_del(cdev);
	class_destroy(monitor_class);
	unregister_chrdev(monitor_major, DEVICE_NAME);
	

	printk(KERN_ALERT "Unloaded.\n");
}




static int monitor_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg) {
	
	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "monitor_ioctl\n");
	printk(KERN_ALERT "command: %d.\n", cmd);
	#endif
	
	switch(cmd){
		case MONITOR_REGISTER:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Registering domain.\n");
			#endif
			monitor_register((( monitor_uspace_info_t*)arg));
			return 0;

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



/*static int monitor_mapmem_pfn(monitor_pfn_report *monitor_pfn_report_t){*/
/*	*/
/*	#ifdef MONITOR_DEBUG	*/
/*	int count;*/
/*	#endif*/

/*	//Setup empty space for incomming metadata struct*/
/*	curr_monitor_pfn_report_t = kmalloc(sizeof(monitor_pfn_report),0);*/

/*	//Copy the data into kernel space*/
/*	if(copy_from_user(curr_monitor_pfn_report_t,monitor_pfn_report_t,sizeof(monitor_pfn_report))!=0){*/
/*		#ifdef MONITOR_DEBUG*/
/*		printk(KERN_ALERT "Failed to copy PFN collection struct to kernelspace.");*/
/*		#endif*/
/*		return MONITOR_MAPFAILED;*/
/*	}*/
/*	*/
/*	//Setup empty space for incomming pfnlist*/
/*	curr_pfnlist = kmalloc((MONITOR_PFNNUM_SIZE*(curr_monitor_pfn_report_t->pfnlist_length)),0);*/
/*	*/
/*	//Copy the data into kernel space*/
/*	if(copy_from_user(curr_pfnlist,curr_monitor_pfn_report_t->pfnlist,(MONITOR_PFNNUM_SIZE*(curr_monitor_pfn_report_t->pfnlist_length)))!=0){*/
/*		#ifdef MONITOR_DEBUG*/
/*		printk(KERN_ALERT "Failed to copy PFN list to kernelspace.");*/
/*		#endif*/
/*		return MONITOR_MAPFAILED;*/
/*	}*/

/*	#ifdef MONITOR_DEBUG			*/
/*	//Print out to compare*/
/*	for(count = 0; count < curr_monitor_pfn_report_t->pfnlist_length; count++){*/
/*		printk(KERN_ALERT "count: %d, pfn:%lu", count, curr_pfnlist[count]);*/
/*	}*/
/*	printk(KERN_ALERT "size: %d",(int)strlen(curr_monitor_pfn_report_t->uuid));	*/
/*	printk(KERN_ALERT "pid: %u, uuid:%s, domid:%u", curr_monitor_pfn_report_t->process_id, curr_monitor_pfn_report_t->uuid,curr_monitor_pfn_report_t->domid);	*/
/*	#endif*/

/*	//monitor_grant();*/
/*	*/
/*	kfree(curr_monitor_pfn_report_t);*/
/*	kfree(curr_pfnlist);*/
/*	*/
/*	return MONITOR_SUCCESS;*/
/*}*/




static int monitor_register( monitor_uspace_info_t *tmp_info){

	int err;
	struct vm_struct *v_start;
	struct gnttab_map_grant_ref ops;
	struct gnttab_unmap_grant_ref unmap_ops;
	monitor_share_info_t *monitor_share_info;

	monitor_share_info = kmalloc(sizeof(monitor_share_info_t),0);

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "monitor_register: got gref:%u domid:%u\n",tmp_info->gref,tmp_info->domid);
	#endif
	
	monitor_share_info->domid = (domid_t)(tmp_info->domid);
	monitor_share_info->gref = (grant_ref_t)(tmp_info->gref);
	monitor_share_info->evtchn = tmp_info->evtchn;


	// The following function reserves a range of kernel address space and
	// allocates pagetables to map that range. No actual mappings are created.
	v_start = alloc_vm_area(PAGE_SIZE);
	if (v_start == 0) {
		free_vm_area(v_start);
		printk(KERN_ALERT "monitor_register: could not allocate page\n");
		return -EFAULT;
	}
	
	/*
	 ops struct in paramaeres
		host_addr, flags, ref
	 ops struct out parameters
		status (zero if OK), handle (used to unmap later), dev_bus_addr
	*/

	//Map in the remote page
	gnttab_set_map_op(&ops, (unsigned long)v_start->addr, GNTMAP_host_map, monitor_share_info->gref, monitor_share_info->domid);

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
		printk(KERN_ALERT "monitor_register:  HYPERVISOR map grant ref failed status = %d\n", ops.status);
		return -EFAULT;
		
	}
	//printk("\nxen: dom0: shared_page = %x, handle = %x, status = %x", (unsigned int)v_start->addr, ops.handle, ops.status);
	// Used for unmapping
	unmap_ops.host_addr = (unsigned long)(v_start->addr);
	unmap_ops.handle = ops.handle;

	//Get a handle on the ring sitting in the page
	sring = (struct as_sring*)v_start->addr;
	BACK_RING_INIT(&(monitor_share_info->bring), sring, PAGE_SIZE);

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "monitor_register: Back ring initialized\n");
	#endif

	//Setup an event channel to the frontend 
	err = bind_interdomain_evtchn_to_irqhandler(monitor_share_info->domid, monitor_share_info->evtchn, monitor_irq_handle, 0, MONITOR_CHANNEL_NAME, monitor_share_info);
	
	if (err < 0) {
		#ifdef MONITOR_DEBUG
		printk(KERN_ALERT "monitor_register: failed binding to evtchn with err: %d\n",err);
		#endif
		err = HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, &unmap_ops, 1);
		return -EFAULT;
	}
	
	//monitor_share_info->irq = err;
	monitor_share_info->evtchn = err;

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "monitor_register: done\n");
	#endif
	
	return 0;
}




/*static int monitor_map_remote_page(int gref){*/

/*	struct vm_struct *v_start;*/

/*	info.gref = gref;*/
/*	info.remoteDomain = 1;*/

/*	// The following function reserves a range of kernel address space and*/
/*	// allocates pagetables to map that range. No actual mappings are created.*/
/*	v_start = alloc_vm_area(PAGE_SIZE);*/
/*	if (v_start == 0) {*/
/*		free_vm_area(v_start);*/
/*		printk("\nxen: dom0: could not allocate page");*/
/*		return -EFAULT;*/
/*	}*/
/*	*/
/*	*/
/*	 ops struct in paramaeres*/
/*		host_addr, flags, ref*/
/*	 ops struct out parameters*/
/*		status (zero if OK), handle (used to unmap later), dev_bus_addr*/
/*	*/
/*	gnttab_set_map_op(&ops, (unsigned long)v_start->addr, GNTMAP_host_map, info.gref, info.remoteDomain);  flags, ref, domID */

/*	if (HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, &ops, 1)) {*/
/*		printk("\nxen: dom0: HYPERVISOR map grant ref failed");*/
/*		return -EFAULT;*/
/*	}*/
/*	if (ops.status) {*/
/*		printk("\nxen: dom0: HYPERVISOR map grant ref failed status = %d", ops.status);*/
/*		return -EFAULT;*/
/*	}*/
/*	printk("\nxen: dom0: shared_page = %x, handle = %x, status = %x", (unsigned int)v_start->addr, ops.handle, ops.status);*/
/*	// Used for unmapping*/
/*	unmap_ops.host_addr = (unsigned long)(v_start->addr);*/
/*	unmap_ops.handle = ops.handle;*/
/*	*/
/*	printk("\nBytes in page ");*/
/*	for(i=0;i<=10;i++) {*/
/*		printk("%c", ((char*)(v_start->addr))[i]);*/
/*	}*/
/*	*/
/*	sring = (as_sring_t*)v_start->addr;*/
/*	BACK_RING_INIT(&info.ring, sring, PAGE_SIZE);*/

/*	 Seetup an event channel to the frontend */
/*	err = bind_interdomain_evtchn_to_irqhandler(info.remoteDomain,*/
/*		    info.evtchn, as_int, 0, "dom0-backend", &info);*/
/*	if (err < 0) {*/
/*		printk("\nxen: dom0: init_module failed binding to evtchn !");*/
/*		err = HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref,*/
/*		      &unmap_ops, 1);*/
/*		return -EFAULT;*/
/*	}*/
/*	*/
/*	info.irq = err;*/
/**/

/*	return 0;*/


/*}*/




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

	unsigned long addr_start;

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "Dom0: Report Received:\n");
	printk(KERN_ALERT "	process_id: %u\n",rep->process_id);
	printk(KERN_ALERT "	domid: %u\n",rep->domid);
	printk(KERN_ALERT "	pfn_list_length: %u\n",rep->pfn_list_length);
	//printk(KERN_ALERT "	pfn_list:	");
	int i;
	/*
	for(i=0; i < rep->pfn_list_length; i++){
		printk(KERN_ALERT "%x",rep->pfn_list[i]);
	}
	*/
	#endif

	addr_start = monitor_map_process(rep);
	//build the report properly
	//monitor_populate_report(rep);


	//Do analysis




	//Unmap RANGE

	//Kill process
	//return MONITOR_RING_KILL;

	//Dont Kill process
	return MONITOR_RING_RESUME;


}


static unsigned long monitor_unmap_range(unsigned long addr_start, int length, int blocksize){


}


static void monitor_populate_report(process_report_t *rep){

	struct vm_struct* v_start;
	struct gnttab_map_grant_ref ops;
	unsigned int *new_gref_list;
	unsigned int *next_gref;
	gref_list_t* new_list;
	int i;
	printk(KERN_EMERG "-1\n");
	new_gref_list = kmalloc(sizeof(unsigned int)*rep->pfn_list_length,0); //make an empty list big enough for all the grefs
	printk(KERN_EMERG "0\n");
	//try mapping the first page of grefs
	next_gref = &(rep->first_gref);

	/*
	
	for ( i = 0; i < rep->pfn_list_length; i++) {

		if(i%MONITOR_GREF_PAGE_COUNT==0){  //If we come to the end of a page boundary

			v_start = alloc_vm_area(PAGE_SIZE);	 // Get a vmarea for a page. No actual mappings are created.
			if (v_start == 0) {
				free_vm_area(v_start);
				printk(KERN_ALERT "populate_report: could not allocate page\n");
				return -EFAULT;
			}
			gnttab_set_map_op(&ops, (maddr_t)((v_start->addr)+(PAGE_SIZE)), GNTMAP_host_map, *next_gref, (rep->domid));
			if (HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, &ops, 1)) {
				printk(KERN_ALERT "populate_report: HYPERVISOR map grant ref failed\n");
				return -EFAULT;
			}
			if (ops.status) {
				printk(KERN_ALERT "populate_report:  HYPERVISOR map grant ref failed status = %d\n", ops.status);
				return -EFAULT;
			}

			new_list = (gref_list_t*)(&v_start); //Interpret the page as a gref_list_t

		}
		new_gref_list[i] = new_list->gref_list[(i%MONITOR_GREF_PAGE_COUNT)];

	}
	rep->gref_list = new_gref_list;
	
	*/
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
	
	

}


static unsigned long monitor_map_process(process_report_t *rep){

	struct vm_struct *v_start; //processes vm area
	struct gnttab_map_grant_ref ops;
	int i;
	printk(KERN_EMERG "Got 1\n");
	// Get a vmarea large enough to hold processes pages. No actual mappings are created.
	v_start = alloc_vm_area((size_t)(PAGE_SIZE*(rep->pfn_list_length)));
	printk(KERN_EMERG "Got 2\n");
	if (v_start == 0) {
		free_vm_area(v_start);
		printk(KERN_ALERT "map_process: could not allocate page\n");
		return -EFAULT;
	}
	printk(KERN_ALERT "map_process: HYPERVISOR");
	//Map in the remote pages one by one

	for (i=0; i < rep->pfn_list_length ; i++){
		printk(KERN_EMERG "Got 3\n");
		BUG_ON(rep->gref_list[i]<0);

		printk(KERN_ALERT "map_process: HYPERVISOR mapping pfn: %d\n",rep->gref_list[i]);
		gnttab_set_map_op(&ops, (phys_addr_t)((v_start->addr)+(i*PAGE_SIZE)), GNTMAP_host_map, rep->gref_list[i], rep->domid);

		if (HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, &ops, 1)) {
			printk(KERN_ALERT "map_process: HYPERVISOR map grant ref failed\n");
			return -EFAULT;
		}
		if (ops.status) {
			printk(KERN_ALERT "map_process:  HYPERVISOR map grant ref failed status = %d\n", ops.status);
			return -EFAULT;
		}
		printk(KERN_ALERT "map_process: mapped pfn %lu\n",rep->pfn_list[i]);
	}

	printk(KERN_ALERT "map_process\n");
	return (unsigned long)v_start->addr;
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


static void monitor_dump_pages(unsigned long *addr, unsigned int numpages){

	unsigned int j;
	unsigned int i;

	for( j = 0; j<MONITOR_DUMP_COUNT && j<numpages; j++){

		//page = mfn_to_virt(mfnlist[j]);

		for( i = 0; i < PAGE_SIZE; i++){
			printk(KERN_ALERT "%x",*(addr+i));
		}
	}

	return;
}



module_init( monitor_init);
module_exit( monitor_exit);
