
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 
#include<linux/uaccess.h>              
#include <linux/ioctl.h>
#include<linux/proc_fs.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("Simple Linux device driver implementing the procfs");
MODULE_VERSION("1.0");

 
#define WR_STRING _IOW('a', 'a', char *)  // Write string command
#define RD_STRING _IOR('a', 'b', char *)  // Read string command

int32_t value = 0;
char etx_array[256]="try_proc_array\n";
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static struct proc_dir_entry *parent;


// Driver Functions 
static long     etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
 
// Procfs Functions 
static ssize_t  read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  write_proc(struct file *filp, const char *buff, size_t len, loff_t * off);


static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .unlocked_ioctl = etx_ioctl,
};


static struct proc_ops proc_fops = {
        .proc_read = read_proc,
        .proc_write = write_proc,
};


static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    if (len > sizeof(etx_array) - 1)  // Leave space for null terminator
        len = sizeof(etx_array) - 1;

    if (copy_from_user(etx_array, buff, len)) {
        pr_err("Proc write failed!\n");
        return -EFAULT;
    }

    etx_array[len] = '\0';  // Ensure null termination
    pr_info("Proc Write: %s\n", etx_array);

    return len;
}

static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length, loff_t * offset)
{
    if (*offset > 0)
        return 0;  // EOF after first read

    if (copy_to_user(buffer, etx_array, strlen(etx_array))) {
        pr_err("Proc read failed!\n");
        return -EFAULT;
    }

    *offset = strlen(etx_array);
    return *offset;   // Return actual bytes read
}



static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case WR_STRING:
        if (copy_from_user(etx_array, (char *)arg, sizeof(etx_array) - 1)) {
            pr_err("Failed to write string via ioctl\n");
            return -EFAULT;
        }
        etx_array[sizeof(etx_array) - 1] = '\0';  // Null terminate
        pr_info("IOCTL Write: %s\n", etx_array);
        break;

        case RD_STRING:
            if (copy_to_user((char *)arg, etx_array, strlen(etx_array) + 1)) {
                pr_err("Failed to read string via ioctl\n");
                return -EFAULT;
            }
            pr_info("IOCTL Read: %s\n", etx_array);
            break;

        default:
            pr_info("Invalid IOCTL command\n");
            return -EINVAL;
    }
    return 0;
}
 

static int __init D6_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            pr_info("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create("etx_class"))){
            pr_info("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device"))){
            pr_info("Cannot create the Device 1\n");
            goto r_device;
        }
        
        /*Create proc directory. It will create a directory under "/proc" */
        parent = proc_mkdir("etx",NULL);
        
        if( parent == NULL )
        {
            pr_info("Error creating proc entry");
            goto r_device;
        }
        
        /*Creating Proc entry under "/proc/etx/" */
        proc_create("etx_proc", 0666, parent, &proc_fops);
 
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
 

static void __exit D6_driver_exit(void)
{
        /* Removes single proc entry */
        //remove_proc_entry("etx/etx_proc", parent);
        /* remove complete /proc/etx */
        proc_remove(parent);
        
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 

module_init(D6_driver_init);
module_exit(D6_driver_exit);
 
