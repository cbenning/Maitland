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
 */

/*

 */


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
#include <xen/evtchn.h>

//File IO, Gross I know, remove this
#include <linux/fcntl.h>
#include <linux/file.h>


//Custom Includes
#include "malpage.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CHRISBENNINGER");

/************************************************************************

Interface and Util Functions

************************************************************************/
static int malpage_init(void) {

	int result = 0;
	struct device *err_dev;

	printk(KERN_ALERT "->malpage_init: Loading...\n");

	if (!xen_domain())
		return -ENODEV;

	//Reserve a major number
	result = alloc_chrdev_region(&malpage_dev, MALPAGE_MIN_MINORS, MALPAGE_MAX_MINORS, DEVICE_NAME);
	malpage_major = MAJOR(malpage_dev);
	malpage_minor = MINOR(malpage_dev);

	if (malpage_major < 0) {
		//printk(KERN_ALERT "Registering the character device failed with major number: %d, minor: %d", malpage_major,malpage_minor);
		printk(KERN_ALERT ">malpage_init: Registering the character device failed with major number: %d\n", malpage_major);
		return -ENODEV;
	}

	//Much simpler, but required udev to run on the machine
	malpage_class = class_create(THIS_MODULE, DEVICE_NAME);

   /* Connect the file operations with the cdev */
	cdev_init(&malpage_cdev, &malpage_fops);
	malpage_cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	if (cdev_add(&malpage_cdev, malpage_dev, 1)) {
		printk(KERN_ALERT "->malpage_init: Failed with registering the character device");
		return 1;
	}

	err_dev = device_create(malpage_class, NULL,malpage_dev,"%s",DEVICE_NAME);

	if (err_dev == NULL) {
		printk(KERN_ALERT ">malpage_init: Registering the character device failed with error: %d",result);
		return -ENODEV;
	}

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "I was assigned major number %d\n", malpage_major);
	printk(KERN_ALERT "PAGE_SHIFT: %d\n", PAGE_SHIFT);
	printk(KERN_ALERT "PAGE_SIZE: %lu\n", PAGE_SIZE);
	printk(KERN_ALERT "gref_list_t size: %lu\n", sizeof(gref_list_t));
	#endif
	
	printk(KERN_ALERT ">malpage_init: Loaded.\n");


	malpage_share_info = malpage_register();

	printk(KERN_ALERT ">malpage_init: Registered.\n");

	return 0;
}





static void malpage_exit(void) {

	printk(KERN_ALERT "->malpage_exit: Unloading...\n");
	
	//malpage_deregister(malpage_share_info);

	/* Unregister the device */
	device_destroy(malpage_class,malpage_dev);
	cdev_del(&malpage_cdev);
	class_destroy(malpage_class);

	//cdev_del(cdev);
	//free_irq(malpage_share_info->evtchn,malpage_share_info->xbdev); //Unecessary
	//unbind_from_irqhandler(malpage_share_info->evtchn,malpage_share_info->xbdev);
	//xenbus_free_evtchn(malpage_share_info->xbdev, malpage_share_info->evtchn);

	printk(KERN_ALERT "Unloaded.\n");
}



/*
Lots of useful stuff in pid.h/pid.c
*/
static int malpage_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg) {

	pid_t procID;
	procID = (pid_t)arg;
	//struct task_struct *task;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "malpage_ioctl got PID: %d.\n", procID);
	printk(KERN_ALERT "command: %d.\n", cmd);
	#endif
	
	switch(cmd){
		case MALPAGE_REPORT:
			#ifdef MALPAGE_DEBUG
			printk(KERN_ALERT "Reporting\n");
			#endif
			return malpage_report(procID,malpage_share_info);
		break;
		case MALPAGE_TEST:
			#ifdef MALPAGE_DEBUG
			printk(KERN_ALERT "Testing\n");
			#endif

			printk(KERN_ALERT "sizeof(unsigned int): %lu\n",sizeof(unsigned int));
			printk(KERN_ALERT "PAGE_SIZE: %lu\n",PAGE_SIZE);
			//Get task_struct for given pid
			/*
			for_each_process(task) {
				if ( task->pid == procID) {
					break;
				}
			}

			#ifdef MALPAGE_DEBUG
			printk(KERN_ALERT "Stopping process %d\n",procID);
			#endif

			malpage_halt_process(task);
			//Generate and store report globally
			malpage_generate_report(task);
			// malpage_store_report(rep);
			*/
			return 0;
		break;
		default:
			#ifdef MALPAGE_DEBUG
			printk(KERN_ALERT "Command not recognized: %d\n", cmd);
			#endif
			return MALPAGE_BADCMD;	
	}
	return 0;
}


/*
static ssize_t malpage_read_gref(struct file *filp, char __user *buffer, size_t count, loff_t *offp){

	unsigned int offset = *offp;
	unsigned int* tmp = kmalloc(count,0);
	unsigned int throwaway;

	if (offset == 0){
		tmp[0] = malpage_share_info->gref;
		tmp[1] = malpage_share_info->evtchn;
		throwaway = copy_to_user(buffer,tmp,count);
		return count;
	}
	else{
		return 0;
	}

}

*/



/*
filp - file pointer
count - size of requested data
buff - pointer to user buffer where data is to be copied
offp - offset pointer into file
*/
/*
static ssize_t malpage_read(struct file *filp, char __user *buffer, size_t count, loff_t *offp){
	
	int bytes_read = 0;
	unsigned int offset = *offp;
	unsigned int tmp_count = count;
	unsigned int leftover = 0;
	struct pfn_ll_node *tmp_pfn_root;
	tmp_pfn_root = pfn_root;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Started read of %u bytes\n",tmp_count);
	printk(KERN_ALERT "Finding offset: %u bytes\n",offset);
	#endif

	//Round down to the nearest 4th-byte
	offset = ((unsigned int)(offset/MALPAGE_PFN_SIZE));
	//Rount count down to the nearest 4th-byte as well
	tmp_count = ((unsigned int)(tmp_count/MALPAGE_PFN_SIZE));

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Normalized count to %u\n",tmp_count);
	printk(KERN_ALERT "Normalized offset to %u\n",offset);
	printk(KERN_ALERT "PFNSIZE: %lu\n",MALPAGE_PFN_SIZE);
	#endif


	//Move through the linked list until tmp_pfn_root points at the desired one.
	while( offset != 0){
		if(!tmp_pfn_root){
			return bytes_read;
		}
		
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "Offset: %d\n",offset);
		#endif
		
		tmp_pfn_root = tmp_pfn_root->next;
		offset--;
	}

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Copying to userspace\n");
	#endif

	while( tmp_count != 0){
	
		if(!tmp_pfn_root){
			return bytes_read;
		}

		//put_user(tmp_pfn_root->pfn, buffer);
		if((leftover = copy_to_user(buffer,tmp_pfn_root,MALPAGE_PFN_SIZE))!=0){
			return bytes_read+(MALPAGE_PFN_SIZE-leftover);
		}
		
		tmp_pfn_root = tmp_pfn_root->next;

		buffer+=MALPAGE_PFN_SIZE;
		tmp_count--;
		bytes_read+=MALPAGE_PFN_SIZE;	
	}
	
	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Copied %d bytes\n",bytes_read);
	#endif
	
	return bytes_read;

}
*/




static int free_pfn_ll(pfn_ll_node *root){

	#ifdef MALPAGE_DEBUG
	int count;	
	#endif
	
	pfn_ll_node *front;
	pfn_ll_node *tmp;
	front = root;
	
	#ifdef MALPAGE_DEBUG
	count = 0;
	#endif
	
	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Started clearing\n");
	#endif
	
	while(front){
	
		tmp = front;
		front = front->next;
		kfree(tmp);
		
		#ifdef MALPAGE_DEBUG
		count++;
		#endif	
	}
	
	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Cleared %d PFNs from list\n", count);
	#endif
	
	return 0;
}



/*
static void pfnlist_show(pfn_ll_node *root){

	pfn_ll_node *front;
	front = root; 
	while(front){
		printk(KERN_ALERT "PFN: %lu\n",front->pfn);
		front = front->next;
	}
	
	return;
}
*/



static int pfnlist_size(pfn_ll_node *root){

	int count;
	pfn_ll_node *front;

	front = root; 
	count = 0;
	while(front){
		count++;
		front = front->next;
	}
	
	return count;
}




static unsigned long* pfnlist_mkarray(pfn_ll_node *root, int length){

	unsigned long *pfn_array = kmalloc(sizeof(unsigned long)*length, 0);
	int count;

	pfn_ll_node *front;
	front = root; 
	count = 0;
	
	while(count < length && front){
		pfn_array[count] = front->pfn;
		front = front->next;
		count++;
	}
	
	return pfn_array;
}




static void pfnlist_mkunique(pfn_ll_node *root){

	pfn_ll_node *front;
	pfn_ll_node *tmp;
	pfn_ll_node *needle_node;
	front = root;
	
	while(front){

		while( (tmp = pfnlist_find_parent_of(front, front->next , front->pfn)) != NULL ){
		
			needle_node = tmp->next;
			tmp->next = needle_node->next;
			kfree(needle_node);
			
		};
		front = front->next;
	}

	return;
	
}




static pfn_ll_node* pfnlist_find_parent_of(pfn_ll_node *parent, pfn_ll_node *haystack, unsigned long needle){

	pfn_ll_node *front;
	pfn_ll_node *front_parent;
	front = haystack;
	front_parent = parent;
	
	while(front){
		if(front->pfn == needle){
			return front_parent;
		}
		front = front->next;
		front_parent = front_parent->next;
	}
	return NULL;
}




static pfn_ll_node* pfnlist(struct task_struct *task, int uniq){

	struct mm_struct *tsk_mm;
	pgd_t *tsk_pgdt;
	//pteval_t tsk_ptev;
	int root_exists;
	int page_all_count;
	//struct task_struct *task;
	
	//Lengths of page tables
	int end_pgd = (PAGE_SIZE/sizeof(pgd_t));
	int end_pmd = (PAGE_SIZE/sizeof(pmd_t));
	int end_pte = (PAGE_SIZE/sizeof(pte_t));
	
	//Temp vars
	int none, present, bad;
	//int file, huge;
	int pgd_count;
	int pmd_count;
	int pte_count;
	pgd_t *tmp_pgdt;
	pmd_t *tmp_pmdt;
	pte_t *tmp_ptet;

	pfn_ll_node *pfn_root;	//Now passed in as param
	pfn_ll_node *tmp_pfn_root;
	pfn_ll_node *tmp_pfn;
	root_exists = 0;
	//page_all_count = 0;
	
	#ifdef MALPAGE_DEBUG		
	printk(KERN_ALERT "pfn_list starting.\n");
	#endif

	//Get the mm object
	tsk_mm = task->mm;
	//Get Page Global Directory
	tsk_pgdt = tsk_mm->pgd;
		
	if(!tsk_mm){
		printk(KERN_ALERT "mm_struct is null\n");
		return (void*)MALPAGE_GENERALERR;
	}
	if(!tsk_pgdt){
		printk(KERN_ALERT "pgd_t is null\n");
		return (void*)MALPAGE_GENERALERR;
	}

	#ifdef MALPAGE_DEBUG		
	printk(KERN_ALERT "pfn_list beginning scan.\n");
	#endif

	//
	//PGD scan
	//
	for(pgd_count = 0; pgd_count < end_pgd; pgd_count++){
		
		tmp_pgdt = (tsk_pgdt+(sizeof(pgd_t)*pgd_count));
		none = pgd_none(*tmp_pgdt);
		present = pgd_present(*tmp_pgdt);
		bad = pgd_bad(*tmp_pgdt);

		//If the PGD Entry actually exists
		if(present && !none && !bad){

			//
			//PMD scan
			//
			for(pmd_count = 0; pmd_count < end_pmd; pmd_count++){

				tmp_pmdt = (pmd_t*)(tmp_pgdt+(sizeof(pmd_t)*pmd_count));
				none = pmd_none(*tmp_pmdt);
				present = pmd_present(*tmp_pmdt);
				bad = pmd_bad(*tmp_pmdt);

				//If the PGD Entry actually exists
				if( present && !none && !bad){

					//
					//PTE scan
					//
					for(pte_count = 0; pte_count < end_pte; pte_count++){

						tmp_ptet = (pte_t*)(tmp_pmdt+(sizeof(pte_t)*pte_count));
						none = pte_none(*tmp_ptet);
						present = pte_present(*tmp_ptet);//Issue here. Aparently a patch fixes a spurious fault caused by this
						//file = pte_file(*tmp_ptet);
						//huge = pte_huge(*tmp_ptet);
						
						//If the PTE Entry actually exists
						if(present && !none){

							//tsk_ptev = pte_flags(*tmp_ptet);
						 	page_all_count++;
							//if(!(tsk_ptev & _PAGE_IOMAP) && (tsk_ptev & _PAGE_USER) && (tsk_ptev & _PAGE_RW) && !(tsk_ptev & _PAGE_HIDDEN) && (tsk_ptev & _PAGE_DIRTY) && (tsk_ptev & _PAGE_ACCESSED)){
							//if(pte_young(*tmp_ptet) && pte_dirty(*tmp_ptet)){

								//Set root if we havent already
								if(!root_exists){
								 	pfn_root = kmalloc(sizeof(pfn_ll_node),0);

								 	//pfn_root->pfn = pteval_to_pfn(tmp_ptet->pte); //This will extract the PFN from the pteval_t
								 	//pfn_root->pfn = pte_pfn(*tmp_ptet);
								 	pfn_root->pfn = pfn_to_mfn(pte_pfn(*tmp_ptet));
								 	pfn_root->next = (void*)NULL;
								 	root_exists = 1;
								 	tmp_pfn_root = pfn_root;

								 	continue;
								}

								tmp_pfn = kmalloc(sizeof(pfn_ll_node),0);
								//Assign current node to the one we just found
								//tmp_pfn->pfn = pteval_to_pfn(tmp_ptet->pte); //This will extract the PFN from the pteval_t

								//tmp_pfn->pfn = pte_pfn(*tmp_ptet); //Original
								tmp_pfn->pfn = pfn_to_mfn(pte_pfn(*tmp_ptet));
								//tmp_pfn->pfn = pfn_to_mfn(page_to_pfn(pte_page(*tmp_ptet)));
								//tmp_pfn->pfn = pte_mfn(*tmp_ptet); //Special Xen function. MFN is what we really want here
								//However only do-able from dom0

								tmp_pfn->next = (void*)NULL;

								//#ifdef MALPAGE_DEBUG
								//printk(KERN_ALERT "found pfn: %lu\n",tmp_pfn->pfn);
								//#endif

								//Attach it to the current root
								tmp_pfn_root->next = tmp_pfn;
								//Advance the pointer
								tmp_pfn_root = tmp_pfn_root->next;
							//}

						}	
					}
				}	
			}
		}
	}

	if(uniq>0){
		pfnlist_mkunique(pfn_root);
	}



	#ifdef MALPAGE_DEBUG		
	printk(KERN_ALERT "pfn_list done, returning.\n");
	printk(KERN_ALERT "found %d pfns.\n",page_all_count);
	#endif
	
	return pfn_root;

}




/************************************************************************

Grant table and Interdomain Functions

************************************************************************/
static int malpage_get_uuid(char* uuid){

	struct file* fd;
	mm_segment_t old_fs;
	unsigned long long offset;

	offset = 0;
	old_fs = get_fs();
	
	set_fs(get_ds()); //Critical otherwise the buffer will be in userspace
	fd = filp_open(MALPAGE_UUID_LOC, O_RDONLY, 0);

	if(IS_ERR(fd)) {
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "malpage_get_uid: error opening: %li.\n",PTR_ERR(fd));
		#endif
		return MALPAGE_SYSFSERR;
    }

	if( vfs_read(fd, uuid, MALPAGE_UUID_LENGTH, &offset ) != MALPAGE_UUID_LENGTH){
			#ifdef MALPAGE_DEBUG		
			printk(KERN_ALERT "malpage_get_uuid problem reading sysfs.\n");
			#endif
			return MALPAGE_SYSFSERR;
	}
	
	set_fs(old_fs);
	filp_close(fd, NULL);
	uuid[MALPAGE_UUID_LENGTH-1] = '\0'; //need to null terminate that bad boy and also remove newline
	
	#ifdef MALPAGE_DEBUG		
	printk(KERN_ALERT "malpage_get_uuid: got %s\n",uuid);
	#endif

	return 0;

}




static int malpage_get_domid(void){

	int result;
	int len;
	void *tmp;
	char *tmp2;
	int value;
	struct xenbus_transaction *xstrans;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "getting domid.\n");
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
	tmp = xenbus_read(*xstrans, MALPAGE_XENSTORE_DOMID_PATH, "", &len);	

	if (IS_ERR(tmp)){
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "error: %li.\n",PTR_ERR(tmp));
		#endif
		return MALPAGE_XSERR;
	}

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "query successful\n");
	#endif

	tmp2 = kmalloc(len,0);
	strncpy(tmp2,(char*)tmp,len);	
	value = simple_strtoul(tmp2,NULL,10);
	
	//Finish up
	result = xenbus_transaction_end(*xstrans, 0);
	kfree(xstrans);
	
	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "got domid %d\n",value);
	#endif
	
	return value;

}




static malpage_share_info_t* malpage_register(void){

	int result;
	char *domid_str;
	char *value;
	struct xenbus_transaction *xstrans;
	malpage_share_info_t *info;
	int ret;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "->malpage_register: building ring share.\n");
	#endif

	//Share ring
	info = kzalloc(sizeof(malpage_share_info_t),GFP_KERNEL);
	info->ring_mfn =  malpage_setup_ring(info);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "->malpage_register:done building ring share\n");
	printk(KERN_ALERT "->malpage_register:beginning creation of xs value\n");
	#endif

	//gref = malpage_share_info->gref;
	info->uuid = kmalloc(MALPAGE_UUID_LENGTH,0);
	malpage_get_uuid(info->uuid);
	info->domid = malpage_get_domid();

	//Ugly hack, but why not?
	value = kmalloc(strlen(MALPAGE_XENSTORE_REGISTER_VALUE_FORMAT)+MALPAGE_UUID_LENGTH+15,0);
	if((ret = sprintf(value, MALPAGE_XENSTORE_REGISTER_VALUE_FORMAT, info->domid, info->gref , info->evtchn, info->uuid)) < 1){
		#ifdef MALPAGE_DEBUG		
		printk(KERN_ALERT "->malpage_register: sprintf broke: %d\n",ret);
		#endif
		//return MALPAGE_GENERALERR;
	}

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "->malpage_register:done creation of xs value with %s\n",value);
	printk(KERN_ALERT "->malpage_register:beginning registration\n");
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
	ret = xenbus_write(*xstrans, MALPAGE_XENSTORE_REGISTER_PATH, domid_str, value);

	//Finish up
	result = xenbus_transaction_end(*xstrans, 0);

	//Clean up
	kfree(value);
	kfree(xstrans);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "->malpage_register: done registration\n");
	#endif
	
	return info;

}

/*
static void malpage_deregister(malpage_share_info_t *info){

	malpage_free_ring(info);

}
*/

static unsigned long malpage_setup_ring(malpage_share_info_t *info){

	struct as_sring *sring;
	int err;

	#ifdef MALPAGE_DEBUG	
	printk(KERN_ALERT "->malpage_setup_ring: ringsetup allocating page\n");
	#endif
	
	info->gref = MALPAGE_GRANT_INVALID_REF;

	sring = (struct as_sring*)__get_free_page(GFP_NOIO | __GFP_HIGH);
	if (!sring) {
		printk(KERN_ALERT "->malpage_setup_ring: Error allocating ring");
		//xenbus_dev_fatal(dev, -ENOMEM, "allocating shared ring");
		return -ENOMEM;
	}

	#ifdef MALPAGE_DEBUG	
	printk(KERN_ALERT "->malpage_setup_ring: setting page as shared ring\n");
	#endif
	
	/* Put a shared ring structure on this page */
	SHARED_RING_INIT(sring);
	FRONT_RING_INIT(&info->fring, sring, PAGE_SIZE);

	#ifdef MALPAGE_DEBUG	
	printk(KERN_ALERT "ringsetup initializing shared ring\n");
	#endif

	err = gnttab_grant_foreign_access(MALPAGE_DOM0_ID, virt_to_mfn(info->fring.sring), 0);
	//err = xenbus_grant_ring(dev, virt_to_mfn(info->fring.sring));

	if (err < 0) {
		free_page((unsigned long)sring);
		info->fring.sring = NULL;
		goto fail;
	}
	info->gref = err;

	//err = xenbus_alloc_evtchn(dev, &info->evtchn);
	err = malpage_alloc_evtchn(info->domid, &info->evtchn); //Wrote my own
	if (err)
		goto fail;

	err = bind_evtchn_to_irqhandler(info->evtchn, malpage_irq_handle, 0, MALPAGE_CHANNEL_NAME, info);
	if (err <= 0) {
		//xenbus_dev_fatal(dev, err, "");
		printk(KERN_ALERT "->genshmf_setup_ring: bind_evtchn_to_irqhandler failed");
		goto fail;
	}
	info->irq = err;

	return 0;
fail:
	malpage_free_ring(info);
	return err;

}


static int malpage_alloc_evtchn(int domid, int *port){

         struct evtchn_alloc_unbound alloc_unbound;
         int err;

         alloc_unbound.dom = DOMID_SELF;
         alloc_unbound.remote_dom = domid;

         err = HYPERVISOR_event_channel_op(EVTCHNOP_alloc_unbound, &alloc_unbound);
         if (err)
				 printk(KERN_ALERT "->malpage_alloc_evtchn: Error allocating event channel");
         else
                 *port = alloc_unbound.port;

         return err;
}


static int malpage_free_evtchn(int port){

		struct evtchn_close close;
		int err;

        close.port = port;

        err = HYPERVISOR_event_channel_op(EVTCHNOP_close, &close);
        if (err)
				printk(KERN_ALERT "->malpage_free_evtchn: Error freeing event channel %d", port);

        return err;
}



static void malpage_free_ring(malpage_share_info_t *info){

	if (info->irq)
		unbind_from_irqhandler(info->irq, info);

	info->evtchn = info->irq = 0;
}



static unsigned int malpage_grant_mfn(unsigned long mfn){

	unsigned int gref;

	//printk(KERN_ALERT "malpage_grant_mfn: %ul",mfn);
	gref = gnttab_grant_foreign_access(MALPAGE_DOM0_ID, mfn, 0);

	if (gref < 0) {
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "grant: could not grant foreign access");
		#endif
		//free_page((unsigned long)mfn_to_virt(mfn));
		return MALPAGE_GRANTERR;
	}
	printk(KERN_ALERT "malpage_grant_mfn: new gref %u",gref);

	#ifdef MALPAGE_DEBUG
	//printk(KERN_ALERT "Finished granting mfn: %ul\n", mfn);
	#endif

	return gref;
}

/*
static void malpage_ungrant_mfn(unsigned long mfn, int gref){

	printk(KERN_ALERT "FIXME 2.1 %d, %lu\n",gref, mfn);
	//gnttab_end_foreign_access(gref, 0, mfn);
	printk(KERN_ALERT "FIXME 2.2\n");
	//free_page((unsigned long)mfn_to_virt(mfn));
	printk(KERN_ALERT "FIXME 2.3\n");
	//gnttab_end_foreign_access(gref, 0, (unsigned long)mfn_to_virt(mfn));

	return;
}*/

 

//Gives you a malloc'ed item, dont forget to clean up when your done
static process_report_t* malpage_generate_report(struct task_struct *task) {

	pfn_ll_node *tmp_root;
	process_report_t *rep;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Generating report\n");
	#endif

	//Get empty report
	rep = kmalloc(sizeof(process_report_t),0);
	rep->process_id = task->pid;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Getting pfn list.\n");
	#endif

	//Make the pfn list
	//tmp_root = (void*)NULL;
	tmp_root = pfnlist(task, 1);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Calculating number of pfns.\n");
	#endif

	rep->pfn_list_length = pfnlist_size(tmp_root);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Report Summary... pfn_list_length: %u\n",rep->pfn_list_length);
	#endif

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Converting ll of pfns to array.\n");
	#endif

	rep->pfn_list = pfnlist_mkarray(tmp_root, rep->pfn_list_length);
	free_pfn_ll(tmp_root);

	rep->gref_list = kmalloc(sizeof(unsigned int)*rep->pfn_list_length, 0);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Determining process age.\n");
	#endif

	rep->process_age = 0;

	//	mfn_to_virt(malpage_shared_mfn)


	return rep;
}


static int malpage_xs_report(process_report_t *rep){


	struct xenbus_transaction *xstrans;
	char *pfn_str,*report_gref_path,*gref_str,*domid_str,*report_path, *pid_str;
	int result;
	int i;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "->malpage_xs_report\n");
	#endif

	xstrans = kmalloc(sizeof(struct xenbus_transaction),0);
	result = xenbus_transaction_start(xstrans);

	//Get a string version of the domid to use in the path
	domid_str = kzalloc(strlen("10000"),0);
	sprintf(domid_str, "%u", rep->domid);

	pid_str = kzalloc(strlen("100000"),0);
	sprintf(pid_str, "%u", rep->process_id);


	//Put grefs and frams nums in XS
	//ULONG_MAX: 18446744073709551615

	report_path = kzalloc(strlen(MALPAGE_XS_REPORT_PATH)+strlen(domid_str),0);
	if((result = sprintf(report_path, "%s/%s",MALPAGE_XS_REPORT_PATH, domid_str)) < 1){
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "->malpage_xs_report: sprintf broke: %d\n",result);
		#endif
		return MALPAGE_GENERALERR;
	}
/*
	report_pfn_path = kzalloc(strlen(MALPAGE_XS_REPORT_PATH)+strlen(domid_str)+strlen(MALPAGE_XS_REPORT_FRAME_PATH),0);
	if((result = sprintf(report_pfn_path, "%s/%s/%s",MALPAGE_XS_REPORT_PATH, domid_str, MALPAGE_XS_REPORT_FRAME_PATH)) < 1){
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "->malpage_xs_report: sprintf broke: %d\n",result);
		#endif
		return MALPAGE_GENERALERR;
	}

	//Make gref dir
	result = xenbus_write(*xstrans, report_path, MALPAGE_XS_REPORT_FRAME_PATH, "1");
*/

	report_gref_path = kzalloc(strlen(MALPAGE_XS_REPORT_PATH)+strlen(domid_str)+strlen(MALPAGE_XS_REPORT_GREF_PATH),0);
	if((result = sprintf(report_gref_path, "%s/%s/%s",MALPAGE_XS_REPORT_PATH, domid_str, MALPAGE_XS_REPORT_GREF_PATH)) < 1){
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "->malpage_xs_report: sprintf broke: %d\n",result);
		#endif
		return MALPAGE_GENERALERR;
	}

	//Make frame dir
	result = xenbus_mkdir(*xstrans, report_path, MALPAGE_XS_REPORT_GREF_PATH);

	pfn_str = kzalloc(strlen("18446744073709551615"),0);
	gref_str = kzalloc(strlen("10000"),0);

	for(i=0; i < rep->pfn_list_length; i ++){

		sprintf(pfn_str, "%lu",rep->pfn_list[i]);

		//Signal report is finished
		//result = xenbus_write(*xstrans, report_pfn_path, pfn_str, pfn_str);

		sprintf(gref_str, "%u",rep->gref_list[i]);

		//Signal report is finished
		result = xenbus_write(*xstrans, report_gref_path, gref_str, pfn_str);
	}


	//Write domid
	result = xenbus_write(*xstrans, report_path, MALPAGE_XS_REPORT_DOMID_PATH, domid_str);

	//write pid
	result = xenbus_write(*xstrans, report_path, MALPAGE_XS_REPORT_PID_PATH, pid_str);



	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "->malpage_xs_report: Finished writing to %s/%s\n",report_path,MALPAGE_XS_REPORT_READY_PATH);
	#endif

	//Signal report is finished
	result = xenbus_write(*xstrans, report_path, MALPAGE_XS_REPORT_READY_PATH, "1");

	//Finish up
	result = xenbus_transaction_end(*xstrans, 0);

	//Clean up
	kfree(report_path);
	kfree(domid_str);
	kfree(xstrans);

	return 0;

}


static int malpage_report(pid_t procID,malpage_share_info_t *info) {

	//struct request_t *req;
	process_report_t *rep;
	struct pid *taskpid;
	int i;
	struct task_struct *task;


	for_each_process(task) {
		if ( task->pid == procID) {
			break;
		}
	}

	taskpid = find_get_pid(procID);

	//Pause it
	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Stopping process %d\n",procID);
	#endif
	malpage_halt_process(task);

	//Generate report
	rep = malpage_generate_report(task);
	rep->domid = info->domid;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "DomU: Report Generated:\n");
	printk(KERN_ALERT "	process_id: %u\n",rep->process_id);
	printk(KERN_ALERT "	domid: %u\n",rep->domid);
	printk(KERN_ALERT "	pfn_list_length: %u\n",rep->pfn_list_length);
	printk(KERN_ALERT "	pfn_list:	");

	/*for(j=0; j < rep->pfn_list_length; j++){
		printk(KERN_ALERT "%ul",rep->pfn_list[j]);
	}*/
	#endif

	/*
	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Dumping processes pages\n");
	#endif
	malpage_dump_pages(rep->pfn_list,rep->pfn_list_length);
	 */

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Granting process pages\n");
	#endif

	//Grant all of the pfn's in the report to Dom0 and save the gref in the report
	for ( i = 0; i < rep->pfn_list_length; i++) {
		rep->gref_list[i] = malpage_grant_mfn(rep->pfn_list[i]);
	}

	#ifdef MALPAGE_DEBUG
	malpage_dump_file(rep);
	#endif

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Storing report in XS\n");
	#endif

	malpage_xs_report(rep);

	return 0;

}

static int malpage_dump_file(process_report_t *rep){
	
	char* dump_filename;
	struct file *file;
	int fd;
	int i;
	void* data;
	loff_t pos = 0;
	mm_segment_t old_fs;

	dump_filename = kzalloc(strlen(MALPAGE_DUMP_FILENAME)+10,0);
	sprintf(dump_filename, MALPAGE_DUMP_FILENAME, rep->process_id);
	printk(KERN_ALERT "Debugging Enabled, Dumping process memory to %s\n",dump_filename);
	
	old_fs = get_fs();
	set_fs(get_ds());
//	fd = sys_open(dump_filename, O_WRONLY|O_CREAT, 0644);
//	if (fd >= 0) {

	file = filp_open(dump_filename, O_WRONLY|O_CREAT, 0644);

	if(file){
	
		//Loop over each PFN
		for(i=0; i < rep->pfn_list_length; i++){
		
			//ignore any that reference the null page
			if(rep->pfn_list[i] == 0){
				continue;
			}
			
			data = pfn_to_kaddr(rep->pfn_list[i]);
			
			if(data==NULL){
				continue;
			}
	
			//Write
			printk(KERN_ALERT "Writing page: %d\n",i);		

	   		vfs_write(file, data, PAGE_SIZE, &pos);
			pos = pos+PAGE_SIZE;
			
		}
		filp_close(file,NULL);
	}
	set_fs(old_fs);

	kfree(dump_filename);

	return 0;
}

static int malpage_op_process(unsigned int op, unsigned int pid){

	struct task_struct *task;

	//Get task_struct for given pid
	for_each_process(task) {
		if ( task->pid == pid) {
			break;
		}
	}

	if(task == NULL){
		return -1;
	}

	if(op == MALPAGE_RING_KILL){
		malpage_kill_process(task);
		return 1;
	}
	else if(op == MALPAGE_RING_RESUME){
		malpage_resume_process(task);
		return 1;
	}
	else if(op == MALPAGE_RING_HALT){
		malpage_halt_process(task);
		return 1;
	}

	return -1;

}


static void malpage_halt_process(struct task_struct *task) {

	//stop it
	force_sig(SIGSTOP, task);

	return;
}


static void malpage_resume_process(struct task_struct *task) {

	//start it
	force_sig(SIGCONT, task);

	return;
}


static void malpage_kill_process(struct task_struct *task) {

	//kill it
	force_sig(SIGKILL, task);

	return;
}


 
/*
 Our interrupt handler for event channel that we set up
 */
static irqreturn_t malpage_irq_handle(int irq, void *dev_id) {

	struct response_t *resp;
    RING_IDX rc, rp;

	#ifdef MALPAGE_DEBUG	
	printk(KERN_ALERT "DomU: Handling Event\n");
	#endif

	again:

		rp = malpage_share_info->fring.sring->rsp_prod;

		for(rc=malpage_share_info->fring.rsp_cons; rc != rp; rc++) {

			resp = RING_GET_RESPONSE(&(malpage_share_info->fring), rc);

			switch(resp->operation) {

				case MALPAGE_RING_NONOP:
					printk(KERN_ALERT  "\nMalpage, Got NONOP: %d\n", resp->operation);
					break;

				case MALPAGE_RING_KILL:
					printk(KERN_ALERT  "\nMalpage, Got KILLOP: %d\n", resp->operation);
					malpage_op_process(MALPAGE_RING_KILL,(resp->report).process_id);
					//TODO: UNMAP Process memory
					break;

				case MALPAGE_RING_RESUME:
					printk(KERN_ALERT  "\nMalpage, Got RESUMEOP: %d\n", resp->operation);
					malpage_op_process(MALPAGE_RING_RESUME,(resp->report).process_id);
					break;
					//TODO: UNMAP Process memory

				case MALPAGE_RING_HALT:
					printk(KERN_ALERT  "\nMalpage, Got PAUSEOP: %d\n", resp->operation);
					malpage_op_process(MALPAGE_RING_HALT,(resp->report).process_id);
					break;

				default:
					printk(KERN_ALERT  "\nMalpage, Unrecognized operation: %d\n", resp->operation);
					break;

			}
		}

		malpage_share_info->fring.rsp_cons = rc;
		if (rc != malpage_share_info->fring.req_prod_pvt) {
			int notify;
			RING_FINAL_CHECK_FOR_RESPONSES(&(malpage_share_info->fring), notify);
			if(notify)
				  goto again;
		} else{
			malpage_share_info->fring.sring->rsp_event = rc+1;
		}

		return IRQ_HANDLED;

}


/*



static void malpage_store_report(process_report_t *rep){

	process_report_node_t *new;
	process_report_node_t *tmp;

	new = kmalloc(sizeof(process_report_node_t),0);
	new->cur = rep;


	//Init list
	if(report_list == NULL){

		report_list = new;
		new->prev = NULL;
		new->next = NULL;

	}
	else{
		tmp = report_list;
		while(tmp->next != NULL){
			tmp = tmp->next;
		}

		//Append to the list
		tmp->next = new;
		new->prev = tmp;
		new->next = NULL;
	}
	return;

}


static process_report_t* malpage_retrieve_report(pid_t procID){

	process_report_node_t *tmp;

	if(report_list == NULL){
		return NULL;
	}

	tmp = report_list;
	while(tmp->next != NULL){

		//Found it
		if(tmp->cur->process_id == procID){
			return tmp->cur;
		}

		tmp = tmp->next;
	}
	return NULL;
}


static void malpage_delete_report(pid_t procID){

	process_report_node_t *tmp;

	if(report_list == NULL){
		return;
	}

	tmp = report_list;

	do{
		//Found it
		if(tmp->cur->process_id == procID){

			//Was Root
			if(tmp->prev == NULL){
				report_list = tmp->next;
				report_list->prev = NULL;
				kfree(tmp->cur);
				kfree(tmp);
			}
			//Was end node
			else if(tmp->next == NULL){
				tmp->prev->next = NULL;
				kfree(tmp->cur);
				kfree(tmp);
			}
			//Was middle node
			else if(tmp->next != NULL && tmp->prev != NULL){
				tmp->prev->next = tmp->next;
				tmp->next->prev = tmp->prev;
				kfree(tmp->cur);
				kfree(tmp);
			}

			return;
		}

		tmp = tmp->next;
	}while(tmp != NULL);

	return;
}



static void malpage_dump_pages(unsigned long* mfnlist, unsigned int len){

	unsigned int* page;
	unsigned int j;
	unsigned int i;

	for( j = 0; j<MALPAGE_DUMP_COUNT && j<len; j++){

		page = mfn_to_virt(mfnlist[j]);

		for( i = 0; i < PAGE_SIZE; i++){
			printk(KERN_ALERT "%x",*(page+i));
		}
	}

	return;
}


*/


module_init( malpage_init);
module_exit( malpage_exit);

