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

	printk(KERN_ALERT "Loading...\n");

	//Reserve a major number
	result = alloc_chrdev_region(&malpage_dev, MALPAGE_MIN_MINORS, MALPAGE_MAX_MINORS, DEVICE_NAME);
	malpage_major = MAJOR(malpage_dev);
	malpage_minor = MINOR(malpage_dev);

	if (malpage_major < 0) {
		//printk(KERN_ALERT "Registering the character device failed with major number: %d, minor: %d", malpage_major,malpage_minor);
		printk(KERN_ALERT "Registering the character device failed with major number: %d\n", malpage_major);
		return -ENODEV;
	}

	//Much simpler, but required udev to run on the machine
	malpage_class = class_create(THIS_MODULE, DEVICE_NAME);
	err_dev = device_create(malpage_class, NULL,malpage_dev,"%s",DEVICE_NAME);

	if (err_dev == NULL) {
		printk(KERN_ALERT "Registering the character device failed with error: %d",result);
		return -ENODEV;
	}

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "I was assigned major number %d\n", malpage_major);
	printk(KERN_ALERT "PAGE_SHIFT: %d\n", PAGE_SHIFT);
	printk(KERN_ALERT "PAGE_SIZE: %lu\n", PAGE_SIZE);
	printk(KERN_ALERT "gref_list_t size: %lu\n", sizeof(gref_list_t));
	#endif
	
	printk(KERN_ALERT "Loaded.\n");

	malpage_register();
	printk(KERN_ALERT "Registered.\n");

	return 0;
}





static void malpage_exit(void) {

	printk(KERN_ALERT "Unloading...\n");
	
	malpage_deregister();
	/* Unregister the device */
	device_destroy(malpage_class,malpage_dev);
	class_destroy(malpage_class);
	unregister_chrdev(malpage_major, DEVICE_NAME);
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
	struct task_struct *task;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "malpage_ioctl got PID: %d.\n", procID);
	printk(KERN_ALERT "command: %d.\n", cmd);
	#endif
	
	switch(cmd){
		case MALPAGE_REGISTER:
			#ifdef MALPAGE_DEBUG
			printk(KERN_ALERT "Registering\n");
			#endif
			return malpage_register();
		break;
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

			printk(KERN_ALERT "sizeof(unsigned int): %u\n",sizeof(unsigned int));
			printk(KERN_ALERT "PAGE_SIZE: %u\n",PAGE_SIZE);
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
	pteval_t tsk_ptev;
	int root_exists;
	int page_all_count;
	//struct task_struct *task;
	
	//Lengths of page tables
	int end_pgd = (PAGE_SIZE/sizeof(pgd_t));
	int end_pmd = (PAGE_SIZE/sizeof(pmd_t));
	int end_pte = (PAGE_SIZE/sizeof(pte_t));
	
	//Temp vars
	int none, present, bad;
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
		if(!none && !bad && present){

			//
			//PMD scan
			//
			for(pmd_count = 0; pmd_count < end_pmd; pmd_count++){

				tmp_pmdt = (pmd_t*)(tmp_pgdt+(sizeof(pmd_t)*pmd_count));
				none = pmd_none(*tmp_pmdt);
				present = pmd_present(*tmp_pmdt);
				bad = pmd_bad(*tmp_pmdt);

				//If the PGD Entry actually exists
				if(!none && !bad && present){

					//
					//PTE scan
					//
					for(pte_count = 0; pte_count < end_pte; pte_count++){
													
						tmp_ptet = (pte_t*)(tmp_pmdt+(sizeof(pte_t)*pte_count));					
						none = pte_none(*tmp_ptet);
						present = pte_present(*tmp_ptet);

						
						//If the PTE Entry actually exists
						if(!none && present){
						
							tsk_ptev = pte_flags(*tmp_ptet);
						 	page_all_count++;
							//if(!(tsk_ptev & _PAGE_IOMAP) && (tsk_ptev & _PAGE_USER) && (tsk_ptev & _PAGE_RW) && !(tsk_ptev & _PAGE_HIDDEN) && (tsk_ptev & _PAGE_DIRTY) && (tsk_ptev & _PAGE_ACCESSED)){
							//if(pte_young(*tmp_ptet) && pte_dirty(*tmp_ptet)){

								//Set root if we havent already
								if(!root_exists){
								 	pfn_root = kmalloc(sizeof(pfn_ll_node),0);
								 	//pfn_root->pfn = pteval_to_pfn(tmp_ptet->pte); //This will extract the PFN from the pteval_t
								 	pfn_root->pfn = pte_pfn(*tmp_ptet);
								 	pfn_root->next = (void*)NULL;
								 	root_exists = 1;
								 	tmp_pfn_root = pfn_root;
								 	continue;
								}

								tmp_pfn = kmalloc(sizeof(pfn_ll_node),0);
								//Assign current node to the one we just found
								//tmp_pfn->pfn = pteval_to_pfn(tmp_ptet->pte); //This will extract the PFN from the pteval_t
								tmp_pfn->pfn = pte_pfn(*tmp_ptet);
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




static int malpage_register(void){

	char *uuid;
	int gref;
	int evtchn;
	int result;
	int domid;
	char *domid_str;
	//void *tmp;
	char *value;
	struct xenbus_transaction *xstrans;
	int ret;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "building ring share.\n");
	#endif

	//Share ring
	kfree(malpage_share_info);
	malpage_share_info = kmalloc(sizeof(malpage_share_info_t),0);
	malpage_shared_mfn = malpage_setup_ring(malpage_share_info);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "done building ring share\n");
	printk(KERN_ALERT "beginning creation of xs value\n");
	#endif

	gref = malpage_share_info->gref;
	uuid = kmalloc(MALPAGE_UUID_LENGTH,0);
	malpage_get_uuid(uuid);
	evtchn = malpage_share_info->evtchn;
	domid = malpage_get_domid();

	printk(KERN_ALERT "FIXME: 3");

	//Ugly hack, but why not?
	value = kmalloc(strlen(MALPAGE_XENSTORE_REGISTER_VALUE_FORMAT)+MALPAGE_UUID_LENGTH+15,0);
	if((ret = sprintf(value, MALPAGE_XENSTORE_REGISTER_VALUE_FORMAT, domid, gref, evtchn, uuid)) < 1){
		#ifdef MALPAGE_DEBUG		
		printk(KERN_ALERT "malpage_register: sprintf broke: %d\n",ret);
		#endif
		return MALPAGE_GENERALERR;
	}

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "done creation of xs value with %s\n",value);
	printk(KERN_ALERT "beginning registration\n");
	#endif
	
	xstrans = kmalloc(sizeof(struct xenbus_transaction),0);
	result = xenbus_transaction_start(xstrans);

	//Get a string version of the domid to use in the path
	domid_str = kmalloc(strlen("10000"),0);
	sprintf(domid_str, "%u", domid);
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
	kfree(uuid);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "done registration\n");
	#endif
	
	return 0;

}


static void malpage_deregister(void){

	malpage_destroy_ring(malpage_share_info);

}


static unsigned long malpage_setup_ring(malpage_share_info_t *info){

	unsigned long mfn;
	struct as_sring *sring;
	unsigned long page;

	#ifdef MALPAGE_DEBUG	
	printk(KERN_ALERT "ringsetup allocating page\n");
	#endif
	
	page = __get_free_pages(GFP_KERNEL, 1);
	if (page == 0) {
		#ifdef MALPAGE_DEBUG	
		printk(KERN_ALERT "ringsetup could not get free page\n");
		#endif
		return 0;
	}

	#ifdef MALPAGE_DEBUG	
	printk(KERN_ALERT "ringsetup setting page as shared ring\n");
	#endif
	
	/* Put a shared ring structure on this page */
	sring = (struct as_sring*)(page);
	SHARED_RING_INIT(sring);

	#ifdef MALPAGE_DEBUG	
	printk(KERN_ALERT "ringsetup initializing shared ring\n");
	#endif

	/* info.ring is the front_ring structure */
	FRONT_RING_INIT(&(info->fring), sring, PAGE_SIZE);            	
	mfn = virt_to_mfn((unsigned int*)page);

	#ifdef MALPAGE_DEBUG	
	printk(KERN_ALERT "ringsetup granting mfn of shared page\n");
	#endif

	malpage_grant_ring(mfn, info);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "ringsetup returning with mfn of shared page\n");
	#endif

	return mfn;

}


static void malpage_destroy_ring(malpage_share_info_t *info){

	printk(KERN_ALERT "FIXME 0\n");
	malpage_ungrant_ring(malpage_shared_mfn,info);
	printk(KERN_ALERT "FIXME 1\n");
	//kfree(mfn_to_virt(malpage_shared_mfn));
	printk(KERN_ALERT "FIXME 2\n");
}



static int malpage_grant_mfn(unsigned long mfn){

	int gref;

	printk(KERN_ALERT "malpage_grant_mfn: %ul",mfn);
	gref = gnttab_grant_foreign_access(MALPAGE_DOM0_ID, mfn, 0);

	if (gref < 0) {
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "grant: could not grant foreign access");
		#endif
		//free_page((unsigned long)mfn_to_virt(mfn));
		return MALPAGE_GRANTERR;
	}

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Finished granting mfn: %ul\n", mfn);
	#endif

	return gref;
}


static void malpage_ungrant_mfn(unsigned long mfn, int gref){

	printk(KERN_ALERT "FIXME 2.1 %d, %ul\n",gref, mfn);
	//gnttab_end_foreign_access(gref, 0, mfn);
	printk(KERN_ALERT "FIXME 2.2\n");
	//free_page((unsigned long)mfn_to_virt(mfn));
	printk(KERN_ALERT "FIXME 2.3\n");
	//gnttab_end_foreign_access(gref, 0, (unsigned long)mfn_to_virt(mfn));

	return;
}

 
static int malpage_grant_ring(unsigned long mfn, malpage_share_info_t *info){

	int err;
	struct xenbus_device *xbdev;
//	info->gref = gnttab_grant_foreign_access(MALPAGE_DOM0_ID, mfn, 0);
//
//
//	if (info->gref < 0) {
//		#ifdef MALPAGE_DEBUG
//		printk(KERN_ALERT "grant: could not grant foreign access");
//		#endif
//		free_page((unsigned long)mfn_to_virt(mfn));
//		return MALPAGE_GRANTERR;
//	}

	info->gref = malpage_grant_mfn(mfn);

	/*
	bind_evtchn_to_irqhandler()
	unbind_from_irqhandler
	 */

	// Setup an event channel to Dom0
	//err = bind_listening_port_to_irqhandler(MALPAGE_DOM0_ID, malpage_irq_handle, 0, MALPAGE_CHANNEL_NAME, info); //Deprecated
	//err = xenbus_alloc_evtchn(MALPAGE_DOM0_ID, info->evtchn);
	xbdev = to_xenbus_device(malpage_dev);
	err = xenbus_alloc_evtchn(&xbdev, &(info->evtchn));
	//bind_evtchn_to_irqhandler(info->evtchn, malpage_irq_handle, 0, MALPAGE_CHANNEL_NAME, info);


/*

	if (err < 0) {
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "grant: could not allocate event channel");
		#endif
		gnttab_end_foreign_access(info->gref, 0, (unsigned long)mfn_to_virt(mfn));
		return MALPAGE_EVTCHANERR ;
	}
*/

	//bind_listening_port_to_irqhandler(unsigned int remote_domain, const char *devname, driver_intr_t handler, void *arg, unsigned long irqflags, unsigned int *irqp)
	/*err = bind_listening_port_to_irqhandler(MALPAGE_DOM0_ID,MALPAGE_CHANNEL_NAME,malpage_irq_handle,0,0,info->evtchn);
	if (err < 0) {
		printk(KERN_ALERT "grant: could not setup event channel");

	}
*/

	err = bind_evtchn_to_irqhandler(info->evtchn, malpage_irq_handle, 0, MALPAGE_CHANNEL_NAME, &malpage_dev);

	if (err < 0) {
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "grant: could not setup event channel");
		#endif
		//gnttab_end_foreign_access(info->gref, 0, (unsigned long)mfn_to_virt(mfn));
		return MALPAGE_EVTCHANERR ;
	}


	//info->irq = err;
	//info->evtchn = err;
//	info->evtchn = irq_to_evtchn_port(info->irq);

	#ifdef MALPAGE_DEBUG
	//printk(KERN_ALERT  "   interupt = %d, local-evtchn = %d", info->irq, info->evtchn);
	printk(KERN_ALERT  "evtchn = %d", info->evtchn);
	#endif

	return 0;

}


static int malpage_ungrant_ring(unsigned long mfn, malpage_share_info_t *info){

	printk(KERN_ALERT "FIXME 1.0\n");
	malpage_ungrant_mfn(mfn, info->gref);
	printk(KERN_ALERT "FIXME 1.1 %u\n",irq_from_evtchn(info->evtchn));
	unbind_from_irqhandler(irq_from_evtchn(info->evtchn), &malpage_dev);
	printk(KERN_ALERT "FIXME 1.2\n");
	return 0;
}



//Gives you a malloc'ed item, dont forget to clean up when your done
static process_report_t* malpage_generate_report(struct task_struct *task) {

	pfn_ll_node *tmp_root;
	process_report_t *rep;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Generating report\n");
	#endif

	//Get empty report
	rep = kmalloc(sizeof(process_report_t),0);
	rep->process_id =  task->pid;

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


static int malpage_report(pid_t procID,malpage_share_info_t *info) {

	struct request_t *req;
	process_report_t *rep;
	int notify;
	int i;
	int j;
	struct task_struct *task;
	gref_list_t* new_list;
	unsigned int tempInt;
	unsigned int tempPFN;
	unsigned int *last_link;

	//Get task_struct for given pid
	for_each_process(task) {
		if ( task->pid == procID) {
			break;
		}
	}

	//Pause it
	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "Stopping process %d\n",procID);
	#endif
	malpage_halt_process(task);

	//Generate and store report
	rep = malpage_generate_report(task);
	//malpage_store_report(rep);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "DomU: Report Generated:\n");
	printk(KERN_ALERT "	process_id: %u\n",rep->process_id);
	printk(KERN_ALERT "	domid: %u\n",rep->domid);
	printk(KERN_ALERT "	pfn_list_length: %u\n",rep->pfn_list_length);
	printk(KERN_ALERT "	pfn_list:	");

	for(j=0; j < rep->pfn_list_length; j++){
		printk(KERN_ALERT "%ul",rep->pfn_list[j]);
	}
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
	printk(KERN_ALERT "Storing grefs in gref_list_t\n");
	#endif

	/*
	tempInt = get_zeroed_page(0); //Get an empty page
	new_list = (gref_list_t*)(&tempInt); //Interpret the page as a gref_list_t
	tempPFN = virt_to_pfn((void*)new_list);  //get the pfn
	rep->first_gref = malpage_grant_mfn(tempPFN);  //grant it, put the gref in the report
	*/
	last_link = &(rep->first_gref);

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "1\n");
	#endif
	
	/*
	for ( i = 0; i < rep->pfn_list_length; i++) {
		//Populate the gref list


		#ifdef MALPAGE_DEBUG
		printk(KERN_EMERG "2\n");
		#endif
		if(i%MALPAGE_GREF_PAGE_COUNT==0){  //If we come to the end of a page boundary
				//tempPage = alloc_page(0);
				tempInt = get_zeroed_page(0); //Get an empty page

				#ifdef MALPAGE_DEBUG
				printk(KERN_EMERG "3\n");
				#endif
				new_list = (gref_list_t*)(&tempInt); //Interpret the page as a gref_list_t
				tempPFN = virt_to_mfn((void*)new_list);  //get the pfn
				#ifdef MALPAGE_DEBUG
				printk(KERN_EMERG "4\n");
				#endif
				*(last_link) = malpage_grant_mfn(tempPFN);  //grant it, note the gref
				#ifdef MALPAGE_DEBUG
				printk(KERN_EMERG "5\n");
				#endif
				last_link = &(new_list->next_gref); //advance the pointer
		}
		#ifdef MALPAGE_DEBUG
		printk(KERN_EMERG "6\n");
		#endif
		new_list->gref_list[(i%MALPAGE_GREF_PAGE_COUNT)] = rep->gref_list[i];
		#ifdef MALPAGE_DEBUG
		printk(KERN_EMERG "7\n");
		#endif
	}
	*(last_link) = 0;  //put a zero so we know there are no more gref lists

	*/
	
	
	tempInt = get_zeroed_page(0); //Get an empty page
	int firstInt = 142;
	int lastInt = 2689;
	new_list = (gref_list_t*)(&tempInt);
	
	memcpy(new_list,&firstInt,sizeof(int));
	memcpy(new_list+(PAGE_SIZE-sizeof(int)),&lastInt,sizeof(int));
	
	tempPFN = virt_to_mfn((void*)new_list);  //get the pfn
	*(last_link) = malpage_grant_mfn(tempPFN);  //grant it, note the gref
	
	//Get an empty request
	req = RING_GET_REQUEST(&(info->fring), info->fring.req_prod_pvt);
	req->operation = MALPAGE_RING_REPORT;

	//Put our generated report into the ring
	req->report = *rep;
	info->fring.req_prod_pvt += 1;

	//Send event
	RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&(info->fring), notify);
    if (notify) {
          printk("\nSent a req to Dom0\n");
          notify_remote_via_irq(info->evtchn);
    } else {
          printk("\nNo notify req to Dom0\n");
          notify_remote_via_irq(info->evtchn);
    }
    printk(KERN_ALERT "2\n");
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
	struct response_t *ring_resp;
	RING_IDX i, rp;

	#ifdef MALPAGE_DEBUG	
	printk(KERN_ALERT "DomU: Interrupt handler called");
	#endif
	
	again:
	rp = info.ring.sring->rsp_prod;
	
	printk("\nxen:DomU: ring pointers %d to %d", info.ring.rsp_cons, rp);
	for(i=info.ring.rsp_cons; i != rp; i++) {
		unsigned long id;
		// what did we get from Dom0
		ring_resp = RING_GET_RESPONSE(&(info.ring), i);
		printk("\nxen:DomU: Recvd in IDX-%d, with id=%d, op=%d, st=%d",
		i, ring_resp->id, ring_resp->operation, ring_resp->status);
		id = ring_resp->id;
		switch(ring_resp->operation) {
		case 0:
		      printk("\nxen:DomU: operation:0");
		      break;
		default:
		      break;
		}
	}
	info.ring.rsp_cons = i;
	if (i != info.ring.req_prod_pvt) {
		int more_to_do;
		RING_FINAL_CHECK_FOR_RESPONSES(&info.ring, more_to_do);
		if(more_to_do)
		      goto again;
	} else
		info.ring.sring->rsp_event = i+1;
	return IRQ_HANDLED;
}
 */
 





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



static void malpage_cleanup_grant(malpage_share_info_t *info, unsigned long pfn){

	int mfn;

	#ifdef MALPAGE_DEBUG
	printk(KERN_ALERT "DomU: destroying grant\n");
	#endif

	mfn = pfn_to_mfn(pfn);

	if(info==NULL){
		return;
	}

	if (gnttab_query_foreign_access(info->gref) == 0) {

		//Remove the grant to the page
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "DomU: Frame had not been mapped\n");
		#endif
		// If 3rd param is non NULL, page has to be freed
		gnttab_end_foreign_access(info->gref, 0, mfn);
		//free_pages(page,1);

	} else {

		//Remove the grant to the page
		#ifdef MALPAGE_DEBUG
		printk(KERN_ALERT "DomU: Frame had been mapped\n");
		#endif
		// Guess, we still free the page, since we are rmmod-ed
		gnttab_end_foreign_access(info->gref, 0, mfn);

	}

	return;
}
*/


module_init( malpage_init);
module_exit( malpage_exit);

