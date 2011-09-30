/******************************************************************************
 * monitor.h
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

//defines
#define DEVICE_NAME "monitor"
#define MONITOR_CHANNEL_NAME "malpage"

//Maxnum of minor numbers
#define MONITOR_MIN_MINORS 0
#define MONITOR_MAX_MINORS 2

#define MONITOR_PFNNUM_SIZE sizeof(unsigned long)

/* Use 'm' as magic number */
#define MONITOR_IOC_MAGIC 270

//IOCTL commands
#define MONITOR_REPORT MONITOR_IOC_MAGIC+1
#define MONITOR_REGISTER MONITOR_IOC_MAGIC+8
#define MONITOR_DEREGISTER MONITOR_IOC_MAGIC+9
#define MONITOR_WATCH MONITOR_IOC_MAGIC+10
#define MONITOR_DUMP MONITOR_IOC_MAGIC+11
#define MONITOR_RESUME MONITOR_IOC_MAGIC+12
#define MONITOR_KILL MONITOR_IOC_MAGIC+13
#define MONITOR_DONE_REPORT MONITOR_IOC_MAGIC+14

//Debug enabled
#define MONITOR_DEBUG 1

//Return Codes
#define MONITOR_SUCCESS 0
#define MONITOR_BADCMD -10
#define MONITOR_MAPFAILED -11
#define MONITOR_XSERR -14
#define MONITOR_ALLOCERR -15
//#define MONITOR_DOMID_RESOLVE_FAILED -12

//Operations for the ring communication
#define MONITOR_RING_REPORT 1
#define MONITOR_RING_NONOP 2
#define MONITOR_RING_KILL 3
#define MONITOR_RING_RESUME 4
#define MONITOR_RING_HALT 5
#define MONITOR_RING_MMUUPDATE 6
#define MONITOR_RING_NX 7
#define MONITOR_RING_NXVIOLATION 8

//Storage constants
#define MONITOR_GREF_PAGE_COUNT (PAGE_SIZE/sizeof(unsigned int))-1

//Length of the uuid
#define MONITOR_UUID_LENGTH sizeof("00000000-0000-0000-0000-000000000000\n")

//Location in Xenstore of domid
#define MONITOR_XENSTORE_DOMID_PATH "/vm/%s/device/console/0/frontend-id"

//Location in Xenstore of custom register node
#define MONITOR_XENSTORE_REGISTER_PATH "/vm/malpage"
#define MONITOR_XENSTORE_REGISTER_NODE "register"

#define MONITOR_VMSTRUCT_SIZE PAGE_SIZE*2
#define MONITOR_GNTTAB_SIZE 10000

#define MONITOR_MAX_VMS 256
#define MONITOR_MAX_PROCS 65536
//#define MONITOR_MAX_PFNS max_pfn
//#define MONITOR_MAX_PFNS ULONG_MAX

#define MONITOR_64_MMUPTR_TYPE_MASK 7ul //For ignoring the last 4 bits

/************************************************************************
Module Interface and Util Structs
************************************************************************/
typedef struct process_report_t{
	unsigned int process_id;
	unsigned int domid;
	unsigned int process_age;
	unsigned long *pfn_list;
	unsigned int *gref_list;
	unsigned int pfn_list_length;
}process_report_t;

typedef struct process_watchreport_t{
	unsigned int process_id;
	unsigned int domid;
	unsigned int process_age;
}process_watchreport_t;

typedef struct gref_list_t{
	unsigned int gref_list[MONITOR_GREF_PAGE_COUNT]; //Fill up te page, leave room for last gref;
	unsigned int next_gref;
}gref_list_t;

/************************************************************************
Grant table and Interdomain Structs
************************************************************************/

typedef struct pfn_page_buffer_t{
	unsigned int next_gref;
	unsigned int grefs[(PAGE_SIZE/sizeof(unsigned int))-1];
}pfn_page_buffer_t;

struct request_t {
	unsigned int operation;
	unsigned int pfn_gref;
	unsigned int pfn;
	process_report_t report;
	unsigned int process_id;
	int domid;
	//uint64_t mmu_ptr;
	//unsigned long* mmu_ptr;
	pte_t* mmu_ptr;
	unsigned long fault_addr;
	//uint64_t mmu_val;
	pte_t mmu_val;
};

struct response_t {
	unsigned int operation;
	unsigned int pfn_gref;
	unsigned int pfn;
	process_report_t report;
	unsigned int process_id;
	int domid;
	//uint64_t mmu_ptr;
	//unsigned long* mmu_ptr;
	pte_t* mmu_ptr;
	unsigned long fault_addr;
	//uint64_t mmu_val;
	pte_t mmu_val;
};

// The following defines the types to be used in the shared ring
DEFINE_RING_TYPES(as, struct request_t, struct response_t);

typedef struct monitor_uspace_info_t {
	unsigned int domid;	
	unsigned int gref;
	unsigned int evtchn;
	unsigned char uuid[MONITOR_UUID_LENGTH];
} monitor_uspace_info_t;

typedef struct monitor_share_info_t {
	domid_t domid;
	grant_ref_t gref;
	unsigned int evtchn;
	unsigned int irq;
	struct as_back_ring bring;
} monitor_share_info_t;


/************************************************************************
Interface and Util Variables
************************************************************************/
static int monitor_major = 0;
static int monitor_minor = 0;
static dev_t monitor_dev;
static struct cdev monitor_cdev;
static struct class* monitor_class;
static struct vm_struct** vm_struct_list;
static int vm_struct_list_size;
static int report_in_progress;

/************************************************************************
Grant table and Interdomain Variables
************************************************************************/
static struct as_sring *sring;
static monitor_share_info_t *monitor_share_info;
static unsigned long ***monitor_dom_list;
static unsigned int curr_proc;


/************************************************************************
Interface and Util Functions
************************************************************************/
static int monitor_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
static int monitor_register(monitor_share_info_t *info);
//static int monitor_check_mmuupdate(unsigned long* mmu_ptr, uint64_t mmu_val, int domid, unsigned int process_id);
static int monitor_check_mmuupdate(pte_t* mmu_ptr, pte_t mmu_val, int domid, unsigned int process_id);
static int monitor_check_page_fault(unsigned int domid, unsigned int process_id, unsigned long address);
static void monitor_print_watched(void);


/************************************************************************
Grant table and Interdomain Functions
************************************************************************/
static irqreturn_t monitor_irq_handle(int irq, void *dev_id);
//static void cleanup_grant(void);
static int monitor_report(process_report_t *rep);
static int monitor_watch(unsigned long arg);
//static unsigned long monitor_unmap_range(unsigned long addr_start, int length, int blocksize);
static struct vm_struct* monitor_map_gref(unsigned int gref, unsigned int domid);
//static void monitor_dump_pages(unsigned long* mfnlist, unsigned int len);
static process_report_t* monitor_populate_report(unsigned long arg);
static monitor_share_info_t* monitor_populate_info(unsigned long arg);
static ssize_t monitor_read(struct file *filp, char *buffer, size_t count, loff_t *offp);
static int monitor_op_process(unsigned int op, unsigned int pid);
static void monitor_halt_process(unsigned int pid);
static void monitor_resume_process(unsigned int pid);
static void monitor_kill_process(unsigned int pid);
static unsigned long* monitor_machine_to_virt(unsigned long maddr);

/************************************************************************
Kernel module bindings
************************************************************************/
static struct file_operations monitor_fops = {
    owner:	THIS_MODULE,
    read:	monitor_read,
//    write:	NULL,
    ioctl:	monitor_ioctl,
//    open:	NULL,
//    release:	NULL,
//    mmap:	NULL,
};



