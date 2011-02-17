/*
 *  process_stop.c
 *
 *  Created on: May 25, 2010
 *  Author: AndiB
 *  Purpose: Module identifies a process and stoppes, continues, or kills the process immediately 
 */

/*Linux includes*/ // I probably don't need all theses => look into it NOW!!!
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/pid.h>
#include <asm/pgtable_types.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <asm/uaccess.h>

/*Xen includes*/
#include <xen/interface/platform.h>
#include <xen/driver_util.h>
#include <xen/grant_table.h>
//#include <xen/xenbus.h>
//#include <tools/include/xs.h>
//#include <xen/xenbus.h>
//#include <xen/interface/xen.h>
//#include <xen/interface/version.h>


/*Custom includes*/
#include "process_stop.h"
/*structs*/
static struct file_operations process_stop_fops = {
    owner:	THIS_MODULE,
    read:	process_stop_read,
    write:	NULL,
    ioctl:	process_stop_ioctl,
    open:	NULL,
    release:	NULL,
    mmap:	NULL,
};

MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");




/* Entry Point for Kernel Module.
 * Creates device and FS access points.
 * returns 0 if success */
static int __init process_stop_init(void) {
	
	int result;
	result = 0;
	

	/*not used by me	

	//if (!is_running_on_xen()) {
	//	printk(KERN_ALERT "Did not detect Xen on system. Load failed.\n");
    //	return -ENODEV;
    //}

	*/
	 

	printk(KERN_ALERT "\nLoading 'process_stop': ... a small program for humankind, but a big step for me\n");

	// take the first free major number that is available on the system
	if((major_num = register_chrdev(0, DEVICE_NAME, &process_stop_fops)) < SUCCESS) {

		printk(KERN_ALERT "***\nfailed to register the device %s", DEVICE_NAME);
		printk(KERN_ALERT "\treturn value is %i\n***\n", major_num);

	}
	
	// get minor number
	minor_num = MINOR(process_stop_dev);

	//Create empty device
	cdev = cdev_alloc();
	

	/*broken code 

	//Populate the device
//	cdev_init(cdev, &process_stop_fops);
//  	cdev->owner = THIS_MODULE;
  	
  	//Create a node in the filesystem corresponding to this device
//	kobject_set_name(&cdev->kobj, "%s", DEVICE_NAME);
//	result = cdev_add(cdev, process_stop_dev, PROCESS_STOP_MAX_MINORS);
//	if (result) {
//		printk(KERN_ALERT "Registering the character device failed with error: %d",result);
//		return -ENODEV;
//	}

	*/
	
	process_stop_class = get_xen_class();
//	device_create(process_stop_class, NULL, process_stop_dev, "%s", DEVICE_NAME);
	
	#ifdef MONITOR_DEBUG
	printk(KERN_ALERT "I was assigned major number %d\n", process_stop_major);
	printk(KERN_ALERT "PAGE_SHIFT: %d\n", PAGE_SHIFT);
	printk(KERN_ALERT "PAGE_SIZE: %lu\n", PAGE_SIZE);
	#endif
	
	printk(KERN_ALERT "Loaded.\n");
	return SUCCESS; /*success*/
	
}




/* __exit function, cleanup FS and devices created in the startup.
 * Cannot fail.
 **/
static void process_stop_exit(void) {

	printk(KERN_ALERT "Killing 'process_stop': ... a small program for humankind, but a big step for me\n");

	unregister_chrdev(major_num, DEVICE_NAME);
	//device_destroy(process_stop_class, process_stop_dev);
	cdev_del(cdev);
	process_stop_test();

}




/* call this if you just want to test that the module can be interacted with
 * from user space. */
static void process_stop_test() {

	printk(KERN_ALERT "**********");
	printk(KERN_ALERT "wow, check it out this module does work");
	printk(KERN_ALERT "if you see this code and your machine has not crashed, then this was a SUCCESSFUL test of process_stop_test()");
	printk(KERN_ALERT "**********");

}




/** Can I access the use the read function? Connected to FS read operation.*/
static ssize_t process_stop_read(struct file * yy, char * userBuf, size_t size, loff_t * bla) {

	unsigned long val;

	printk(KERN_ALERT "%s",userBuf);
	printk(KERN_ALERT "That was it");

	userBuf[5] = 68;
	val = copy_to_user(userBuf, "hello", size);

	printk(KERN_ALERT "%i", (int) val);
	
	userBuf = "hello";
	
	return 1;

}


static void trigger_sig_cont(struct task_struct *task) {
	force_sig(SIGCONT, task);
}


/** the ACTUAL WORK is done here 
 * Process a IOCTL system call on this device.
 * ARG identifies the process which the command shall be applied to.
 * CMD is the command number, which is offset by a magic value to ensure that
 * IOCTL does not eat it when it passes through.
 *
 * RETURNS code based on operation performed
 *
 * Operations:
 *
 * CMD - 0 - STOP
 * CMD - 1 - continue
 * CMD - 2 - kill
 *  */
static int process_stop_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg) {

	struct task_struct *task;
	pid_t *target_task;
	int *switch_var, *pid_counter;

	switch_var = kmalloc (sizeof(int), GFP_KERNEL | GFP_ATOMIC);
	target_task = kmalloc(sizeof(pid_t), GFP_KERNEL | GFP_ATOMIC);
	pid_counter = kmalloc(sizeof(int), GFP_KERNEL | GFP_ATOMIC);
	task = kmalloc(sizeof(struct task_struct), GFP_KERNEL | GFP_ATOMIC);	

	task = get_current();
	*target_task  = arg;
	*pid_counter = 0;
	*switch_var = -1;

	/* find the task you want to deal with
	*  search through the circular list
	*  with the current task as the head into the list
	*/ 
	do {
		task = next_task(task);

		#ifdef PROCESS_STOP_DEBUG
		printk(KERN_ALERT "process number: %i", (int) task->pid);
		#endif

		if (task->pid == (unsigned)1) {
			(*pid_counter)++;
		}
	} while (task->pid != *target_task && *pid_counter < 2 );

	if(*pid_counter > 1) {
		*switch_var = -2;

	} else { 
		*switch_var = cmd-MAGIC_NUMBER;
	}

	printk(KERN_ALERT "this is switch_var: %i\n",*switch_var);

	switch (*switch_var) {
		//stop
		case 0: 
			force_sig(SIGSTOP, task);
			break;
		// resume 
		// TO DO: modify this, so that the process cannot tell (notify parent, etc.) that it had been stopped....
		case 1: 
			printk(KERN_ALERT "before sigcont\n");
			trigger_sig_cont(task);
			printk(KERN_ALERT "after sigcont\n");
			break;
		// kill forever
		case 2: 
			force_sig(SIGKILL, task);
			break;
		default: 
			#ifdef PROCESS_STOP_DEBUG
			printk(KERN_ALERT "");
			printk(KERN_ALERT "process_stop ioctl(....cmd had unknown/unused value");
			#endif

		//	kfree(switch_var);
		//	kfree(target_task);
		//	kfree(task);

			if(*pid_counter > 1) {
				#ifdef PROCESS_STOP_DEBUG
				printk(KERN_ALERT "");
				printk(KERN_ALERT "process_stop ioctl(....target task pid not found");
				#endif
//				kfree(pid_counter);
				return 11; 
			}
//			kfree(pid_counter);
			return EINVAL;
	}

//	kfree(switch_var);
//	kfree(target_task);
//	kfree(pid_counter);
//	kfree(task);
	return SUCCESS;

}



module_init( process_stop_init);
module_exit( process_stop_exit);

