/*defines*/
#define DEVICE_NAME "process_stop"
#define PROCESS_STOP_MIN_MINORS 0
#define PROCESS_STOP_MAX_MINORS 2
#define MAGIC_NUMBER 900
/*Return Codes*/
#define SUCCESS 0
#define FAILSAFE_LIST_TRAVERSAL 100000
/*************************************
	32767 seems to be the maximum number of PIDs that can concurrenly exist on a system TODAY.
	Assume the PID you wish to affect disappears before the corresponding task_struct has been found.
	Without reusing the PID the module would loop through 4ever. 
	100000 seemed like a nice number to determine safely that the PID no longer exists.
*************************************/


/*static vars*/
static int major_num = 0;
static int minor_num = 0;
static dev_t process_stop_dev;
static struct cdev* cdev;
static struct class* process_stop_class;



/*functions*/
//main way to communicate with module
static int process_stop_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

// will probably be depreciated VERY soon
static ssize_t process_stop_read(struct file *, char *, size_t size, loff_t * bla);

// produces a message in the log (type dmesg to see it if no output appears on screen)
static void process_stop_test(void);


