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
//varibale which is changes through ioctl
static int device_value = 10;

// Function prototypes
static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *offset);
static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *offset);
static long    my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

// File operations structure
static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .read           = my_read,
    .write          = my_write,
    .unlocked_ioctl = my_ioctl,
};

// module initialization function
static int __init D5_driver_init(void){

    // allocating the device number
    if (alloc_chrdev_region(&my_dev, 0, 1, DEVICE_NAME)){
        pr_err("device number allocation unsucessful!\n");
        return -1;
    }

    //create the device class
    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)){
        pr_err("class creation unsucessful");
        goto r_class;
    }

    //device file creation
    if (IS_ERR(device_create(my_class, NULL, my_dev,NULL,DEVICE_NAME))){
        pr_err("Failed to create device\n");
        goto r_device;
    }

    //Initializing the character driver cdev structure and adding it
    cdev_init(&my_cdev, &fops);
    if (cdev_add(&my_cdev, my_dev, 1) < 0) {
        pr_err("Failed to add cdev\n");
        goto r_cdev;
    }

    pr_info("Device driver inserted successfully\n");
    return 0;

    r_cdev:
        device_destroy(my_class, my_dev);
    r_device:
        class_destroy(my_class);
    r_class:
        unregister_chrdev_region(my_dev,1);
        return -1;
}

// module exit function
static void __exit D5_driver_exit(void){
    cdev_del(&my_cdev);
    device_destroy(my_class, my_dev);
    class_destroy(my_class);
    unregister_chrdev_region(my_dev,1);
    pr_info("device driver removed successfully");
}

// read function
static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *offset){
    if (*offset >= BUFFER_SIZE) {
        return 0; // End of buffer
    }

    len = min(len, (size_t)(BUFFER_SIZE - *offset));
    if (copy_to_user(buf, kernel_buffer + *offset, len)) {
        return -EFAULT;
    }

    *offset += len;
    pr_info("Data read: %zu bytes\n", len);
    return len;
}

//write function
static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *offset){
    if (*offset >= BUFFER_SIZE) {
        return -ENOSPC; // No space left
    }

    len = min(len, (size_t)(BUFFER_SIZE - *offset));
    if (copy_from_user(kernel_buffer + *offset, buf, len)) {
        return -EFAULT;
    }

    *offset += len;
    pr_info("Data written: %zu bytes\n", len);
    return len;
}

//ioctl function
static long    my_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    switch (cmd)
    {
        case IOCTL_CLEAR_BUFFER:
            memset(kernel_buffer, 0, BUFFER_SIZE);
            pr_info("Buffer cleared\n");
            break;

        case IOCTL_SET_VALUE:
            if (copy_from_user(&device_value, (int *)arg, sizeof(device_value)))
                return -EFAULT;
            pr_info("Value set to %d\n", device_value);
            break;

        case IOCTL_GET_VALUE:
            if (copy_to_user((int *)arg, &device_value, sizeof(device_value)))
                return -EFAULT;
            pr_info("Value sent: %d\n", device_value);
            break;

        default:
            return -EINVAL; // Invalid command
    }
    return 0;
}


module_init(D5_driver_init);
module_exit(D5_driver_exit);
