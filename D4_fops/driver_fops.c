#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 1024


MODULE_LICENSE("GPL");
MODULE_AUTHOR("surendhar");
MODULE_DESCRIPTION("Character Device Driver with Read and Write");
MODULE_VERSION("1.0");


//variables declaration
dev_t dev = 0;
static struct class *dev_class;
static struct cdev f_cdev;
static char kernel_buffer[BUFFER_SIZE];
static size_t data_size = 0;  // Tracks the amount of written data


/* Read function */
static ssize_t f_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    int ret;
    
    if (*off >= data_size) return 0;  // No more data to read

    if (len > data_size - *off) len = data_size - *off;

    ret = copy_to_user(buf, kernel_buffer + *off, len);
    if (ret != 0) return -EFAULT;

    *off += len;  // Update offset for next read
    pr_info("Read %zu bytes from device\n", len);
    return len;
}


/* Write function */

static ssize_t f_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    int ret;
    if (len > BUFFER_SIZE) len = BUFFER_SIZE;  // Avoid overflow

    ret = copy_from_user(kernel_buffer, buf, len);
    if (ret != 0) return -EFAULT;

    data_size = len;  // Store data size
    pr_info("Wrote %zu bytes to device\n", len);
    return len;
}



/* File operations structure */
static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .read    = f_read,
    .write   = f_write,
};


static int __init f_driver_init(void)
{
    /* Allocate major number */
    if ((alloc_chrdev_region(&dev, 0, 1, "f_Dev")) < 0)
    {
        pr_err("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(dev), MINOR(dev));

    /* Initialize cdev */
    cdev_init(&f_cdev, &fops);

    /* Add cdev to the system */
    if ((cdev_add(&f_cdev, dev, 1)) < 0)
    {
        pr_err("Cannot add the device to the system\n");
        goto r_class;
    }

    /* Create class */
    if (IS_ERR(dev_class = class_create("f_class")))
    {
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }

    /* Create device */
    if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "f_device")))
    {
        pr_err("Cannot create the device\n");
        goto r_device;
    }

    pr_info("Device Driver Inserted Successfully\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, 1);
    return -1;
}

/* Module Exit */
static void __exit f_driver_exit(void)
{
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&f_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Removed\n");
}


module_init(f_driver_init);
module_exit(f_driver_exit);