/******************************************************************************
 * malpage.h
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
 */


//defines
#define DEVICE_NAME "malpage"
#define MALPAGE_CHANNEL_NAME "malpage"
#define MALPAGE_DOM0_ID 0

//Maxnum of minor numbers
#define MALPAGE_MIN_MINORS 0
#define MALPAGE_MAX_MINORS 2

//Maximum number of processes reported at the same time
#define MALPAGE_MAX_REPORTS 255

//Memory size of a pfn
#define MALPAGE_PFN_SIZE sizeof(unsigned long)

//Location in FS of uuid
#define MALPAGE_UUID_LOC "/sys/hypervisor/uuid"
#define MALPAGE_UUID_LENGTH strlen("00000000-0000-0000-0000-000000000000\n")

//Xenstore Paths
#define MALPAGE_XENSTORE_DOMID_PATH "domid"
#define MALPAGE_XENSTORE_REGISTER_PATH "/malpage/register"

//unsigned int domid:unsigned int gref:unsigned int evtchn;
#define MALPAGE_XENSTORE_REGISTER_VALUE_FORMAT "%u:%u:%u:%s"
#define MALPAGE_XS_REPORT_PATH "/malpage/report"
#define MALPAGE_XS_REPORT_READY_PATH "ready"
#define MALPAGE_XS_REPORT_DOMID_PATH "domid"
#define MALPAGE_XS_REPORT_PID_PATH "pid"
#define MALPAGE_XS_REPORT_GREF_PATH "grefs"
#define MALPAGE_XS_REPORT_FRAME_PATH "frames"

#define MALPAGE_XS_WATCHREPORT_PATH "/malpage/watch"

//IOCTL commands
#define MALPAGE_IOC_MAGIC 250 //Magic number
#define MALPAGE_REPORT MALPAGE_IOC_MAGIC+1
#define MALPAGE_PAGEINFO MALPAGE_IOC_MAGIC+2
#define MALPAGE_PFNLIST MALPAGE_IOC_MAGIC+3
#define MALPAGE_PFNCLR MALPAGE_IOC_MAGIC+4
#define MALPAGE_PFNCOUNT MALPAGE_IOC_MAGIC+5
#define MALPAGE_PFNLISTSHOW MALPAGE_IOC_MAGIC+6
#define MALPAGE_RINGREQ MALPAGE_IOC_MAGIC+7
#define MALPAGE_REGISTER MALPAGE_IOC_MAGIC+8
#define MALPAGE_TEST MALPAGE_IOC_MAGIC+9
#define MALPAGE_WATCH MALPAGE_IOC_MAGIC+10

//Operations for the ring communication
#define MALPAGE_RING_REPORT 1
#define MALPAGE_RING_NONOP 2
#define MALPAGE_RING_KILL 3
#define MALPAGE_RING_RESUME 4
#define MALPAGE_RING_HALT 5
#define MALPAGE_RING_MMUUPDATE 6
#define MALPAGE_RING_NX 7
#define MALPAGE_RING_NXVIOLATION 8

//Storage constants
#define MALPAGE_GREF_PAGE_COUNT (PAGE_SIZE/sizeof(unsigned int))-1

//Debug enabled
#define MALPAGE_DEBUG 1

//Error Codes
#define MALPAGE_BADCMD -10
#define MALPAGE_GENERALERR -9
#define MALPAGE_PAGEALLOCERR -11
#define MALPAGE_GRANTERR -12
#define MALPAGE_EVTCHANERR -13
#define MALPAGE_XSERR -14
#define MALPAGE_SYSFSERR -15

//Dump
#define MALPAGE_DUMP_COUNT 10
#define MALPAGE_DUMP_FILENAME "/tmp/%d_dump.bin"

#define MALPAGE_RING_SIZE __RING_SIZE((struct genshm_sring *)0, PAGE_SIZE)
#define MALPAGE_GRANT_INVALID_REF	0
#define MALPAGE_ULONG_STR_MAX 12

#define MALPAGE_64_MMUPTR_SHIFT 4 //For killing the last 4 bits
#define MALPAGE_64_MMUPTR_TYPE_MASK 7ul //For ignoring the last 4 bits

#define MALPAGE_PF_PROT 1 << 0
#define MALPAGE_PF_WRITE 1 << 1
#define MALPAGE_PF_USER 1 << 2
#define MALPAGE_PF_RSVD 1 << 3
#define MALPAGE_PF_INSTR 1 << 4

/************************************************************************
Module Interface and Util Structs
************************************************************************/

//struct for linked list of PFN ids
typedef struct pfn_ll_node{
	unsigned long pfn;
	struct pfn_ll_node *next;	
}pfn_ll_node;


typedef struct process_report_t{
	unsigned int process_id;
	domid_t domid;
	unsigned int process_age;
	unsigned long *pfn_list;
	unsigned int *gref_list;
	unsigned int pfn_list_length;
	unsigned int first_gref;
}process_report_t;


typedef struct gref_list_t{
	unsigned int gref_list[MALPAGE_GREF_PAGE_COUNT]; //Fill up te page, leave room for last gref;
	unsigned int next_gref;
}gref_list_t;


typedef struct process_report_node_t{
	struct process_report_t *cur;
	struct process_report_node_t *next;
	struct process_report_node_t *prev;
}process_report_node_t;

/************************************************************************
Grant table and Interdomain Structs
************************************************************************/

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

//struct for passing between domains via the network
typedef struct malpage_share_info_t {
	domid_t domid;
	grant_ref_t gref;
	unsigned int evtchn;
	unsigned int irq;
	int ring_mfn;
	char *uuid;
	struct as_front_ring fring;

} malpage_share_info_t;

/************************************************************************
Interface and Util Variables
************************************************************************/
static int malpage_major = 0;
static int malpage_minor = 0;
static dev_t malpage_dev;
static struct cdev malpage_cdev;
static struct class* malpage_class;
process_report_node_t *report_list;

static struct semaphore* report_sem;
static pid_t report_pid;
static int report_running = 1;
static int report_in_progress = 0;
static struct task_struct* reporter;

static struct semaphore* process_op_sem;
static unsigned int process_op_pid;
static unsigned int process_op_op;
static int process_op_running = 1;
static struct task_struct* process_oper;



/************************************************************************
Grant table and Interdomain Variables
************************************************************************/
static	malpage_share_info_t *malpage_share_info = NULL;
//static int malpage_share_info_set = 0;
static spinlock_t malpage_mmu_info_lock;

/************************************************************************
Interface and Util Functions
************************************************************************/

extern int (*kmalpage_do_page_fault)(struct task_struct *task, unsigned long address, unsigned long error_code);
static int malpage_init(void);
static void malpage_exit(void);
static int malpage_report_thread(void* args);
static int malpage_process_op_thread(void* args);
static int malpage_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg );
static int malpage_mmu_update(struct mmu_update *req, int count,int *success_count, domid_t domid);
//static int malpage_multi_mmu_update(struct multicall_entry *mcl, struct mmu_update *req, int count,int *success_count, domid_t domid);
static int malpage_multi_mmu_update(pte_t *ptep, pte_t pte);
static int malpage_mmuext_op(struct mmuext_op *op, int count, int *success_count, domid_t domid);
static int malpage_multi_mmuext_op(struct multicall_entry *mcl, struct mmuext_op *op, int count, int *success_count, domid_t domid);
static int malpage_update_descriptor(u64 ma, u64 desc);
static int malpage_multi_update_descriptor(struct multicall_entry *mcl, u64 maddr,struct desc_struct desc);
static int malpage_update_va_mapping(unsigned long va, pte_t new_val, unsigned long flags);
static int malpage_multi_update_va_mapping(struct multicall_entry *mcl, unsigned long va,pte_t new_val, unsigned long flags);
static pfn_ll_node* pfnlist_vmarea(struct task_struct *task, int duplicates,  int anon);
static unsigned long addr_to_mfn(struct mm_struct *mm, unsigned long addr);
static pfn_ll_node* pfnlist(struct task_struct *task, int uniq);
static int pfnlist_size(pfn_ll_node *root);
static int free_pfn_ll(pfn_ll_node *root);
static unsigned long* pfnlist_mkarray(pfn_ll_node *root, int length);
static int pfnlist_mkunique(pfn_ll_node *root);
static pfn_ll_node* pfnlist_find_parent_of(pfn_ll_node *parent, pfn_ll_node *haystack, unsigned long needle);
static int malpage_op_process(unsigned int op, unsigned int pid);
static void malpage_halt_process(struct task_struct *task);
static void malpage_resume_process(struct task_struct *task);
static void malpage_kill_process(struct task_struct *task);
static int malpage_flipnx_page(pte_t *ptep, pte_t pte);
static int malpage_do_page_fault(struct task_struct *task, unsigned long address, unsigned long error_code);
static unsigned long* malpage_machine_to_virt(unsigned long maddr);
static xmaddr_t malpage_arb_virt_to_machine(void *vaddr);
static void* malpage_kzalloc(size_t size);

/************************************************************************
Grant table and Interdomain Functions
************************************************************************/
static int malpage_get_uuid(char* uuid);
static int malpage_get_domid(void);
static malpage_share_info_t* malpage_register(void);
static void malpage_deregister(malpage_share_info_t *info);
static process_report_t* malpage_generate_report(struct task_struct *task);
static int malpage_report(pid_t procID, malpage_share_info_t *info);
static int malpage_watch(pid_t procID, malpage_share_info_t *info);
static unsigned long malpage_setup_ring(malpage_share_info_t *info);
static void malpage_free_ring(malpage_share_info_t *info);
static int malpage_grant_ring(unsigned long mfn, malpage_share_info_t *info);
static int malpage_ungrant_ring(malpage_share_info_t *info);
static irqreturn_t malpage_irq_handle(int irq, void *dev_id);
static unsigned int malpage_grant_mfn(unsigned long mfn);
static void malpage_ungrant_mfn(unsigned long mfn, int gref);
static int malpage_alloc_evtchn(int domid, int *port);
static int malpage_free_evtchn(int port);
static int malpage_xs_report(process_report_t *rep);
static int malpage_dump_file(process_report_t *rep);

/************************************************************************
Kernel module bindings
************************************************************************/
static struct file_operations malpage_fops = {
    owner:	THIS_MODULE,
//    read:	malpage_read,
//    read:	malpage_read_gref,
//    write:	NULL,
    ioctl:	malpage_ioctl,
//    open:	NULL,
//    release:	NULL,
//    mmap:	NULL,
};


