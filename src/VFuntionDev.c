#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#define DEVICE_NAME "vFunctionDev"
#define CLASS_NAME "VFD"
#define MASK 255
#define BYTE 8
#define SIZE_OF_ARRAY 200

extern int charProcessor(char* charVectorFunction,int* vectorFunction, double* intervals);
extern int getAxisValues(int* function, double* intervals, double* results);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Javier Herrera, Kevin Barrantes y Ivan Chavarria");
MODULE_DESCRIPTION("Provides comunication between VFuntion and the GraphicApplication");
MODULE_VERSION("0.1");

//Mis variables
static int w_count = 0;
static int r_count = 0;
static size_t vectorFunctionSize;
static double intervals[16];
static int vectorFunction[SIZE_OF_ARRAY];
static double results[3000];

//Variables básicas
static int majorNumber;
static int numberOpens = 0;
static struct class* modifyClass = NULL;
static struct device* modifyDevice = NULL;

//Definición de funciones
static int d_open(struct inode*, struct file*);
static int d_release(struct inode*, struct file*);
static ssize_t d_read(struct file*, char*, size_t, loff_t*);
static ssize_t d_write(struct file*, const char*, size_t, loff_t*);

static struct file_operations fops =
{
	.open = d_open,
	.read = d_read,
	.write = d_write,
	.release = d_release,
};

static int __init vFunctionDev_init(void)
{
	printk(KERN_INFO "VFunctionDev: Initializing the vFunctionDev\n");
	
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if(majorNumber < 0)
	{
		printk(KERN_ALERT "vFunctionDev: failed to register major number\n");
		return majorNumber;
	}
	printk(KERN_INFO "vFunctionDev: registerred correctly with major number %d\n", majorNumber);
	
	modifyClass = class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR(modifyClass))
	{
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(modifyClass);
	}
	printk(KERN_INFO "vFunctionDev: device class registered correctly\n");
	
	modifyDevice = device_create(modifyClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if(IS_ERR(modifyDevice))
	{
		class_destroy(modifyClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(modifyDevice);
	}
	
	printk(KERN_INFO "vFunctionDev: device class created correctly\n");
	
	return 0;
}

static void __exit vFunctionDev_exit(void){
   device_destroy(modifyClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(modifyClass);                          // unregister the device class
   class_destroy(modifyClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "vFunctionDev: Task acomplished, module is out of scene\n");
}

static int d_open(struct inode* inodep, struct file* filep)
{
	numberOpens++;
	printk(KERN_INFO "vFunctionDev: Device has been opened %d time(s)\n",numberOpens);
	return 0;
}

static ssize_t d_read(struct file* filep, char* buffer, size_t len, loff_t* offset)
{
	//VFunction(vectorFunction, intervals, results);
	(void)copy_to_user(buffer, (char*)results, len);
	++r_count;
	return len;
}

static ssize_t d_write(struct file* filep, const char* buffer, size_t len, loff_t* offset)
{
	unsigned char charArray[len];
	
	(void)copy_from_user(&charArray, buffer, len);
	vectorFunctionSize = charProcessor(charArray, vectorFunction, intervals);	


	getAxisValues(vectorFunction,intervals,results);
	++w_count;
	return vectorFunctionSize;
}

static int d_release(struct inode* inodep, struct file* filep)
{
	printk(KERN_INFO "vFunctionDev: device successfully closed\n");
	return 0;
}

module_init(vFunctionDev_init);
module_exit(vFunctionDev_exit);
