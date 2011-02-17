/******************************************************************************
 * genshmb.h
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
#define DEVICE_NAME "genshm"
#define GENSHMB_CHANNEL_NAME "genshm"

//Maxnum of minor numbers
#define GENSHMB_MIN_MINORS 0
#define GENSHMB_MAX_MINORS 2

#define GENSHMB_PFNNUM_SIZE sizeof(unsigned long)

/* Use 'm' as magic number */
#define GENSHMB_IOC_MAGIC 270

//IOCTL commands
#define GENSHMB_REPORT GENSHMB_IOC_MAGIC+1
#define GENSHMB_REGISTER GENSHMB_IOC_MAGIC+8
#define GENSHMB_DEREGISTER GENSHMB_IOC_MAGIC+9

//Debug enabled
#define GENSHMB_DEBUG 1

//Return Codes
#define GENSHMB_SUCCESS 0
#define GENSHMB_BADCMD -10
#define GENSHMB_MAPFAILED -11
#define GENSHMB_XSERR -14
#define GENSHMB_ALLOCERR -15
//#define GENSHMB_DOMID_RESOLVE_FAILED -12


//Operations for the ring communication
#define GENSHMB_RING_REPORT 1
#define GENSHMB_RING_NONOP 2
#define GENSHMB_RING_KILL 3
#define GENSHMB_RING_RESUME 4
#define GENSHMB_RING_HALT 5

//Storage constants
#define GENSHMB_GREF_PAGE_COUNT (PAGE_SIZE/sizeof(unsigned int))-1

//Length of the uuid
#define GENSHMB_UUID_LENGTH sizeof("00000000-0000-0000-0000-000000000000\n")
#define GENSHMB_DUMP_COUNT 10

/************************************************************************
Module Interface and Util Structs
************************************************************************/
//struct for linked list of PFN ids
typedef struct genshmb_pfn_report{
	unsigned int process_id;
	char uuid[GENSHMB_UUID_LENGTH];
	unsigned int domid;
	unsigned long *pfnlist;
	unsigned int process_age;
	unsigned int pfnlist_length;
}genshmb_pfn_report;


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
	unsigned int gref_list[GENSHMB_GREF_PAGE_COUNT]; //Fill up te page, leave room for last gref;
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
};
struct response_t {
	unsigned int operation;
	unsigned int pfn_gref;
	unsigned int pfn;
	process_report_t report;
};

// The following defines the types to be used in the shared ring
DEFINE_RING_TYPES(genshm, struct request_t, struct response_t);


/************************************************************************
Interface and Util Variables
************************************************************************/
static int genshmb_major = 0;
static int genshmb_minor = 0;
//static genshmb_pfn_report *curr_genshmb_pfn_report_t;
//static unsigned long *curr_pfnlist;
static dev_t genshmb_dev;
struct cdev genshmb_cdev;
static struct class* genshmb_class;


/************************************************************************
Grant table and Interdomain Variables
************************************************************************/
//static genshmb_share_info_t *genshmb_share_info;
/*
static as_request_t request_t;
static as_response_t response_t;
static int gref;
static int port;

*/

struct genshmb_info{
	struct mutex mutex;
	struct xenbus_device *xbdev;
	struct gendisk *gd;
	int vdevice;
	int ring_mfn;
	char *uuid;
	unsigned int domid;
	struct genshm_back_ring bring;
	unsigned int gref;
	unsigned int evtchn, irq;
	int feature_barrier;
	int is_ready;
	struct vm_struct *ring_area;
	spinlock_t io_lock;
};

struct genshmb_uspace{
	unsigned int domid;
	unsigned int gref;
	unsigned int evtchn;
	unsigned char uuid[GENSHMB_UUID_LENGTH];
};

struct genshmb_info *genshm_back_info;

//////////////////////////////////////////////////////////////////////////////
static const struct xenbus_device_id genshmb_ids[] = {
	{ "genshm" },
	{ "" }
};

/************************************************************************
Interface and Util Functions
************************************************************************/
static int genshmb_init(void);
static void genshmb_exit(void);
void genshmb_xenbus_init();
void genshmb_xenbus_exit();
static void genshmf_changed(struct xenbus_device *dev, enum xenbus_state frontend_state);
static int genshmb_remove(struct xenbus_device *dev);
static int genshmb_probe(struct xenbus_device* dev, const struct xenbus_device_id* id);
static irqreturn_t genshmb_interrupt_handler(int irq, void *dev_id);
static int genshmb_connect_ring(struct genshmb_info *be);
static int genshmb_uevent(struct xenbus_device* xdev, char** envp, int num_envp, char* buffer, int buffer_size);
static int genshmb_disconnect_ring(struct genshmb_info *be);
static int genshmb_register(struct genshmb_info *info);
static struct genshmb_info* genshmb_populate_info(unsigned long arg);
static int genshmb_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);


static struct xenbus_driver genshmb_driver = {
	.name = "genshm",
	.owner = THIS_MODULE,
	.ids = genshmb_ids,
	.probe = genshmb_probe,
	.remove = genshmb_remove,
	.uevent  = genshmb_uevent,
	.otherend_changed = genshmf_changed,
};


/************************************************************************
Kernel module bindings
************************************************************************/

static struct file_operations genshmb_fops = {
    owner:	THIS_MODULE,
    read:	NULL,
    write:	NULL,
    ioctl:	genshmb_ioctl,
    open:	NULL,
    release:	NULL,
    mmap:	NULL,
};


