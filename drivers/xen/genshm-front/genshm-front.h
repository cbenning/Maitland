/******************************************************************************
 * genshmf.h
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
#define DEVICE_NAME "genshm"
#define GENSHMF_CHANNEL_NAME "genshm"
#define GENSHMF_DOM0_ID 0

//Maxnum of minor numbers
#define GENSHMF_MIN_MINORS 0
#define GENSHMF_MAX_MINORS 2

//Maximum number of processes reported at the same time
//#define GENSHMF_MAX_REPORTS 255

//Memory size of a pfn
#define GENSHMF_PFN_SIZE sizeof(unsigned long)

//Location in FS of uuid
#define GENSHMF_UUID_LOC "/sys/hypervisor/uuid"
#define GENSHMF_UUID_LENGTH strlen("00000000-0000-0000-0000-000000000000\n")

//Xenstore Paths
#define GENSHMF_XENSTORE_DOMID_PATH "domid"
#define GENSHMF_XENSTORE_REGISTER_PATH "/genshm/register"

//unsigned int domid:unsigned int gref:unsigned int evtchn;
#define GENSHMF_XENSTORE_REGISTER_VALUE_FORMAT "%u:%u:%u:%s"

//IOCTL commands
#define GENSHMF_IOC_MAGIC 250 //Magic number
#define GENSHMF_REPORT GENSHMF_IOC_MAGIC+1
#define GENSHMF_PAGEINFO GENSHMF_IOC_MAGIC+2
#define GENSHMF_PFNLIST GENSHMF_IOC_MAGIC+3
#define GENSHMF_PFNCLR GENSHMF_IOC_MAGIC+4
#define GENSHMF_PFNCOUNT GENSHMF_IOC_MAGIC+5
#define GENSHMF_PFNLISTSHOW GENSHMF_IOC_MAGIC+6
#define GENSHMF_RINGREQ GENSHMF_IOC_MAGIC+7
#define GENSHMF_REGISTER GENSHMF_IOC_MAGIC+8
#define GENSHMF_TEST GENSHMF_IOC_MAGIC+9

//Operations for the ring communication
#define GENSHMF_RING_REPORT 1
#define GENSHMF_RING_NONOP 2
#define GENSHMF_RING_KILL 3
#define GENSHMF_RING_RESUME 4
#define GENSHMF_RING_HALT 5

//Storage constants
#define GENSHMF_GREF_PAGE_COUNT (PAGE_SIZE/sizeof(unsigned int))-1

//Debug enabled
#define GENSHMF_DEBUG 1

//Error Codes
#define GENSHMF_BADCMD -10
#define GENSHMF_GENERALERR -9
#define GENSHMF_PAGEALLOCERR -11
#define GENSHMF_GRANTERR -12
#define GENSHMF_EVTCHANERR -13
#define GENSHMF_XSERR -14
#define GENSHMF_SYSFSERR -15

#define GENSHM_RING_SIZE __RING_SIZE((struct genshm_sring *)0, PAGE_SIZE)
#define GENSHM_GRANT_INVALID_REF	0

/************************************************************************
Interface and Util Variables
************************************************************************/
static int genshmf_major = 0;
static int genshmf_minor = 0;
static dev_t genshmf_dev;
static struct class* genshmf_class;

/************************************************************************
Kernel module bindings
************************************************************************/
struct request_t {
	unsigned int operation;
	unsigned int pfn_gref;
	unsigned int pfn;
	//process_report_t report;
};
struct response_t {
	unsigned int operation;
	unsigned int pfn_gref;
	unsigned int pfn;
	//process_report_t report;
};


// The following defines the types to be used in the shared ring
DEFINE_RING_TYPES(genshm, struct request_t, struct response_t);

struct genshmf_info{
	struct mutex mutex;
	struct xenbus_device *xbdev;
	struct gendisk *gd;
	int vdevice;
	unsigned int gref;
	int ring_mfn;
	char *uuid;
	unsigned int domid;
	struct genshm_front_ring fring;
	unsigned int evtchn, irq;
	int feature_barrier;
	int is_ready;
	spinlock_t io_lock;
};

struct genshmf_info *genshm_front_info;

typedef struct gref_list_t{
	unsigned int gref_list[GENSHMF_GREF_PAGE_COUNT]; //Fill up te page, leave room for last gref;
	unsigned int next_gref;
}gref_list_t;


/************************************************************************
Module Interface and Util Structs
************************************************************************/
static void genshmf_exit(void);
static int genshmf_init(void);
static int genshmf_register(struct genshmf_info* info);
static int genshmf_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
static irqreturn_t genshmf_interrupt_handler(int irq, void *dev_id);
static int genshmf_probe(struct xenbus_device* dev, const struct xenbus_device_id* id);
static void genshmb_changed(struct xenbus_device *dev, enum xenbus_state backend_state);
static void genshmf_free_ring(struct genshmf_info *info);
static int genshmf_init_ring(struct xenbus_device *dev, struct genshmf_info *info);
static int genshmf_setup_ring(struct genshmf_info *info);
static int genshmf_alloc_evtchn(int domid, int *port);
static int genshmf_free_evtchn(int port);
static int genshmf_get_uuid(char* uuid);
static int genshmf_get_domid(void);
//void genshmf_xenbus_init();
//void genshmf_xenbus_exit();


//////////////////////////////////////////////////////////////////////////////
static struct xenbus_device_id genshmf_ids[] = {
	{ "genshm" },
	{ "" }
};

//////////////////////////////////////////////////////////////////////////////

static struct xenbus_driver genshmf_driver = {
	.name = "genshm",
	.owner = THIS_MODULE,
	.ids = genshmf_ids,
	.probe = genshmf_probe,
	.otherend_changed = genshmb_changed,
};




static struct file_operations genshmf_fops = {
    owner:	THIS_MODULE,
    read:	NULL,
    write:	NULL,
    ioctl:	genshmf_ioctl,
    open:	NULL,
    release:	NULL,
    mmap:	NULL,
};


