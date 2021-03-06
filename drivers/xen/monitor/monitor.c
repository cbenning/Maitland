// Maitland: A prototype paravirtualization-based packed malware detection system for Xen virtual machines
// Copyright (C) 2011 Christopher A. Benninger

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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
#include <asm/xen/page.h>
#include <linux/slab.h>


//#include <linux/bootmem.h> //For max_pfn
#include <linux/bitmap.h> //For bitmap
#include <linux/bitops.h>

//Dynamic Arrays
//#include <linux/flex_array.h>
//#include "flex_array.c"

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
	//int i;

	printk(KERN_ALERT "%s:  Loading...\n",__FUNCTION__);

	//Reserve a major number
	result = alloc_chrdev_region(&monitor_dev, MONITOR_MIN_MINORS, MONITOR_MAX_MINORS, DEVICE_NAME);
	monitor_major = MAJOR(monitor_dev);
	monitor_minor = MINOR(monitor_dev);
	
	if (monitor_major < 0) {
		printk(KERN_ALERT "%s: Registering the character device failed with major number: %d, minor: %d\n",__FUNCTION__, monitor_major,monitor_minor);
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
	//printk(KERN_ALERT "max_pfn: %lu\n", max_pfn);
	#endif
		
	printk(KERN_ALERT "->monitor_init: Loaded.\n");
	
	//Init the data store
	monitor_dom_list = kzalloc(sizeof(unsigned long*)*MONITOR_MAX_VMS,GFP_KERNEL);
	//monitor_dom_list = flex_array_alloc(sizeof(struct flex_array*),MONITOR_MAX_VMS,GFP_KERNEL);

    curr_proc = NULL;
    report_in_progress = 0;
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
		case MONITOR_DUMP:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Received Dump\n");
			#endif
			monitor_print_watched();
			return 0;
		break;
		case MONITOR_DONE_REPORT:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Received Done Report\n");
			#endif
			return 0;
		break;
		case MONITOR_RESUME:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Received Resume\n");
			#endif
            if(curr_proc!=NULL){
			    printk(KERN_ALERT "Resuming %u\n",curr_proc);
    			monitor_resume_process(curr_proc);
                curr_proc = NULL;
            }
			return 0;
		break;
		case MONITOR_KILL:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Received Kill\n");
			#endif
			if(curr_proc!=NULL){
    			monitor_kill_process(curr_proc);
                curr_proc = NULL;
            }			
            return 0;
		break;
		case MONITOR_WATCH:
			#ifdef MONITOR_DEBUG
			printk(KERN_ALERT "Received watch report\n");
			#endif
			monitor_watch(arg);
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
	
	info->evtchn = err;

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "->monitor_register: done\n");
	#endif
	
	//Setup this domain with it's own list of processes
	monitor_dom_list[info->domid] = kzalloc(sizeof(unsigned long)*MONITOR_MAX_PROCS,GFP_KERNEL);

	return 0;
}




static irqreturn_t monitor_irq_handle(int irq, void *dev_id){

		RING_IDX rc, rp;
		struct request_t req;
		struct response_t resp;
		int notify;
		monitor_share_info_t *monitor_share_info;
        unsigned long* tmp_ptr;

		#ifdef MONITOR_DEBUG
		//printk(KERN_ALERT "Dom0: Handling Event\n");
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

			// update the req-consumer
			monitor_share_info->bring.req_cons = ++rc;
			barrier();

			switch (req.operation) {

				case MONITOR_RING_REPORT :

				    	//printk(KERN_ALERT "\nMonitor, Got MONITOR_RING_REPORT op: %u", req.operation);
    					//resp.operation = monitor_report(&(req.report));
    					//resp.report = req.report;

                    resp.operation = MONITOR_RING_NONOP;
					break;

				case MONITOR_RING_MMUUPDATE:	

					if(req.domid>0 && req.domid < MONITOR_MAX_VMS){

                        printk(KERN_ALERT "%s: GOT: %d\n",__func__,__LINE__);//FIXME
                        //If the process is one we are watching
						if(monitor_check_mmuupdate(req.mmu_ptr,req.mmu_val,req.domid,req.process_id)>0){

                            printk(KERN_ALERT "%s: MONITOR_RING_MMUUPDATE:%d:%u", __FUNCTION__,req.domid,req.process_id);
                            resp.process_id = req.process_id;
                            resp.domid = req.domid;
                            resp.mmu_ptr = req.mmu_ptr;
                            resp.mmu_val = req.mmu_val;
                            resp.operation = MONITOR_RING_NX; //request mark NX
                            //printk(KERN_ALERT "%s: Request mark page Non-Exec", __FUNCTION__);

                        }
					}
					break;
				case MONITOR_RING_NXVIOLATION:
	
					if(req.domid>0 && req.domid < MONITOR_MAX_VMS){

                        printk(KERN_ALERT "%s: GOT: %d\n",__func__,__LINE__);//FIXME
                        //If the process is one we are watching
						if(!report_in_progress && monitor_check_page_fault(req.domid,req.process_id,req.fault_addr)>0){
 
                            printk(KERN_ALERT "%s: MONITOR_RING_NXVIOLATION:%d:%u", __FUNCTION__,req.domid,req.process_id);
                            resp.process_id = req.process_id;
                            resp.domid = req.domid;
                            resp.operation = MONITOR_RING_REPORT; //request report

                        }
                        else{
                            //monitor_resume_process(req.process_id);                            
                            resp.operation = MONITOR_RING_NONOP; 
                        }
					}
					break;
				default:
					//printk(KERN_ALERT "\nMonitor, Unrecognized operation: %u", req.operation);
                    resp.operation = MONITOR_RING_NONOP; 
					break;
					  
			}

		    //printk(KERN_ALERT "\nMonitor, sending operation: %u", resp.operation);
			memcpy(RING_GET_RESPONSE(&monitor_share_info->bring, monitor_share_info->bring.rsp_prod_pvt), &resp, sizeof(resp));
			monitor_share_info->bring.rsp_prod_pvt++;

			//RING_PUSH_RESPONSES_AND_CHECK_NOTIFY(&monitor_share_info->bring, notify);
			RING_PUSH_RESPONSES(&monitor_share_info->bring);
			notify_remote_via_irq(monitor_share_info->evtchn);
			//RING_FINAL_CHECK_FOR_REQUESTS(&monitor_share_info->bring, notify);
			//printk(KERN_ALERT "Monitor, Sending respnse\n\n");

		}

		return IRQ_HANDLED;
}


static unsigned long* monitor_machine_to_virt(unsigned long maddr){
    unsigned offset = maddr & ~PAGE_MASK;
    return __va(XPADDR(PFN_PHYS(mfn_to_pfn(PFN_DOWN(maddr))) | offset).paddr);
}


static int monitor_report(process_report_t *rep) {

	int i;
	int j;
	struct vm_struct* tmp_vm_struct;

	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "Dom0: Report Received:\n");
	printk(KERN_ALERT "	process_id: %u\n",rep->process_id);
	printk(KERN_ALERT "	domid: %u\n",rep->domid);
	printk(KERN_ALERT "	pfn_list_length: %u\n",rep->pfn_list_length);
	//printk(KERN_ALERT "	pfn_list:	");
	/*for(i=0; i < rep->pfn_list_length; i++){
		printk(KERN_ALERT "%lu",rep->pfn_list[i]);
	}*/
	#endif

	vm_struct_list = kzalloc(rep->pfn_list_length*PAGE_SIZE,0);
	j = 0;
	for(i = 0; i < rep->pfn_list_length; i++){
		tmp_vm_struct = monitor_map_gref(rep->gref_list[i],rep->domid);
		if(tmp_vm_struct->size > 0){
			vm_struct_list[j] = tmp_vm_struct;
			j++;
		}
	}

	vm_struct_list_size = j;

	printk(KERN_ALERT "Successfully mapped %d pages",vm_struct_list_size);

    curr_proc = rep->process_id;
	printk(KERN_ALERT "Setting curr_proc to: %u\n",(unsigned int)curr_proc);
	//Do analysis

	//Unmap RANGE

	//Kill process
	//return MONITOR_RING_KILL;
    //monitor_kill_process(rep->process_id);
    //monitor_resume_process(rep->process_id);

	//Dont Kill process
	//return MONITOR_RING_RESUME;

    return 0;
}




static int monitor_watch(unsigned long arg){

	unsigned long **dom_cursor;
	unsigned long proc_cursor;
	process_watchreport_t *rep, *tmp_rep;

	tmp_rep = (process_watchreport_t*)arg;
	rep = kzalloc(sizeof(process_watchreport_t),GFP_KERNEL);
	
	rep->domid = tmp_rep->domid;
	rep->process_id = tmp_rep->process_id;
	rep->process_age = tmp_rep->process_age;
	dom_cursor = NULL;

	printk(KERN_ALERT "%s, setting watch for proc %d in Dom %d.",__FUNCTION__,rep->process_id,rep->domid);

	dom_cursor = monitor_dom_list[rep->domid];
	if(!dom_cursor){
		printk(KERN_ALERT "%s, Dom: %u is not registered, ignoring watch.",__FUNCTION__,rep->domid);
		return -1;
	}

	dom_cursor[rep->process_id] = 1;
	monitor_dom_list[rep->domid] = dom_cursor;

    //monitor_print_watched();

	return 0;
}

static void monitor_print_watched(void){

	int i,j,k;
	unsigned long **dom_cursor;
	unsigned long proc_cursor;

	printk(KERN_ALERT "%s,Dumping store...",__FUNCTION__);

	for(i=0; i<MONITOR_MAX_VMS;i++){
		dom_cursor = monitor_dom_list[i];

		if(!dom_cursor){
			continue;
		}
		
		printk(KERN_ALERT "--Domain #%d",i);

		for(j=0; j<MONITOR_MAX_PROCS; j++){
			proc_cursor = dom_cursor[j];
			if(proc_cursor){

				printk(KERN_ALERT "----Process #%d",j);
			
			}
		}
	}
    return;
}

//static int monitor_check_mmuupdate(unsigned long* mmu_ptr, uint64_t mmu_val, int domid, unsigned int process_id){
static int monitor_check_mmuupdate(pte_t* mmu_ptr, pte_t mmu_val, int domid, unsigned int process_id){

	unsigned int i;
	unsigned long **dom_cursor;
	unsigned long proc_cursor;

	dom_cursor = monitor_dom_list[domid];
	if(!dom_cursor){
		printk(KERN_ALERT "%s, Dom: %u is not registered, ignoring watch.",__FUNCTION__,domid);
		return -1;
	}		

    //printk(KERN_ALERT "%s,Watched Dom:%u is attempting to change PTE",__FUNCTION__,domid);
    //return 1;

	for(i=0; i < MONITOR_MAX_PROCS; i++){
		proc_cursor = dom_cursor[i];
		if(proc_cursor && i==process_id){

			printk(KERN_ALERT "%s,Watched Process #%d in Dom:%u is attempting to change PTE",__FUNCTION__,i,domid);
			return 1;
		}
	}
	
	//printk(KERN_ALERT "%s,Unwatched process modifying MFN #%lu, ignoring",__FUNCTION__,mmu_mfn);
	return 0;

}


static int monitor_check_page_fault(unsigned int domid, unsigned int process_id, unsigned long address){

	unsigned int i;
	unsigned long **dom_cursor;
	unsigned long proc_cursor;

    
	//printk(KERN_ALERT "%s, Searching for %d: %d\n.",__FUNCTION__,domid,process_id);

	dom_cursor = monitor_dom_list[domid];
	if(!dom_cursor){
		printk(KERN_ALERT "%s, Dom: %u is not registered, ignoring NX violation.",__FUNCTION__,domid);
		return -1;
	}

	for(i=0; i < MONITOR_MAX_PROCS; i++){
		proc_cursor = dom_cursor[i];
		if(proc_cursor && i==process_id){

			printk(KERN_ALERT "%s,Watched Process #%d in Dom:%u is attempting to execute NX page",__FUNCTION__,i,domid);
			return 1;
		}
	}
	
	//printk(KERN_ALERT "%s,Unwatched process, ignoring NX violation",__FUNCTION__,mmu_mfn);
	return 0;

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

}


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

	//printk(KERN_ALERT "monitor_map_gref: HYPERVISOR mapping gref: %d\n",gref);


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
		//printk(KERN_ALERT "monitor_map_gref: test: %d",*((int*)v_start->addr));
		//printk("\nmonitor_map_gref: shared_page = %x, handle = %x, status = %x",(unsigned int)v_start->addr, ops.handle, ops.status);
	}

	return v_start;

}


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

static int monitor_op_process(unsigned int op, unsigned int pid){

    int notify;
    struct response_t resp;
    resp.operation = op;
    resp.process_id = pid;

    memcpy(RING_GET_RESPONSE(&monitor_share_info->bring, monitor_share_info->bring.rsp_prod_pvt), &resp, sizeof(resp));
    monitor_share_info->bring.rsp_prod_pvt++;
    RING_PUSH_RESPONSES_AND_CHECK_NOTIFY(&monitor_share_info->bring, notify);
    notify_remote_via_irq(monitor_share_info->evtchn);
    
    /*
    ring_req = RING_GET_REQUEST(&(monitor_share_info->bring), monitor_share_info->bring.rsp_prod_pvt);
    ring_req->operation = op;
    ring_req->process_id = pid;
    RING_PUSH_REQUESTS(&(monitor_share_info->bring)); 
    notify_remote_via_irq(monitor_share_info->irq);
    */

    return 0;
}


static void monitor_halt_process(unsigned int pid) {
	//stop it
	monitor_op_process(MONITOR_RING_HALT, pid);
	return;
}


static void monitor_resume_process(unsigned int pid) {
	//start it
	monitor_op_process(MONITOR_RING_RESUME, pid);
	return;
}


static void monitor_kill_process(unsigned int pid) {
	//kill it
	monitor_op_process(MONITOR_RING_KILL, pid);
	return;
}


module_init( monitor_init);
module_exit( monitor_exit);


