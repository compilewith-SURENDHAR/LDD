#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

//module configuration
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("Character device driver with IOCTL");
MODULE_VERSION("1.0");

// macros 
#define DEVICE_NAME "D5_device_ioctl"
#define CLASS_NAME  "D5_class_iotcl"
#define BUFFER_SIZE 1024

// IOCTL Command Definitions
#define MAGIC_NUM 100
#define IOCTL_CLEAR_BUFFER _IO(MAGIC_NUM, 1)
#define IOCTL_SET_VALUE    _IOW(MAGIC_NUM, 2, int)
#define IOCTL_GET_VALUE    _IOR(MAGIC_NUM, 3, int)

// Global variables
static dev_t my_dev;
static struct class *my_class;
static struct cdev my_cdev;
static char kernel_buffer[BUFFER_SIZE];
static int device_value = 10;

// Function prototypes
static ssize_t dev_read(struct file *filp, char __user *buf, size_t len, loff_t *offset);
static ssize_t dev_write(struct file *filp, const char __user *buf, size_t len, loff_t *offset);
static long    dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

// File operations structure
static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .read           = dev_read,
    .write          = dev_write,
    .unlocked_ioctl = dev_ioctl,
};

// module initialization function
static int __init D5_driver_init(void){

}

// module exit function
static int __exit D5_driver_exit(void){

}

// read function
static ssize_t dev_read(struct file *filp, char __user *buf, size_t len, loff_t *offset){

}

//write function
static ssize_t dev_write(struct file *filp, const char __user *buf, size_t len, loff_t *offset){

}

//ioctl function
static long    dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    
}


module_init(D5_driver_init);
module_exit(D5_driver_exit);
