#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("Character Device Driver with Workqueue");
MODULE_VERSION("1.0");

#define DEVICE_NAME "workqueue_dev_static"

dev_t dev;
static struct cdev etx_cdev;
static struct class *dev_class;
static struct workqueue_struct *workqueue;

// Work structure
static struct work_struct my_work;

// Workqueue function
static void workqueue_fn(struct work_struct *work) {
    pr_info("Workqueue Function Executing. Process ID: %d\n", current->pid);
}

// File Operations
static int etx_open(struct inode *inode, struct file *file) {
    pr_info("Device Opened\n");
    return 0;
}

static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
    pr_info("Write operation received, scheduling workqueue\n");
    queue_work(workqueue, &my_work);  // Add work to queue
    return len;
}

static int etx_release(struct inode *inode, struct file *file) {
    pr_info("Device Closed\n");
    return 0;
}

// File Operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = etx_open,
    .write = etx_write,
    .release = etx_release,
};

// Module Init
static int __init etx_driver_init(void) {
    if ((alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME)) < 0) {
        pr_err("Cannot allocate major number\n");
        return -1;
    }

    cdev_init(&etx_cdev, &fops);
    if ((cdev_add(&etx_cdev, dev, 1)) < 0) {
        pr_err("Cannot add device to system\n");
        goto r_class;
    }

    if (IS_ERR(dev_class = class_create("etx_class"))) {
        pr_err("Cannot create struct class\n");
        goto r_class;
    }

    if (IS_ERR(device_create(dev_class, NULL, dev, NULL, DEVICE_NAME))) {
        pr_err("Cannot create device\n");
        goto r_device;
    }

    // Create workqueue
    workqueue = create_workqueue("etx_workqueue");
    if (!workqueue) {
        pr_err("Cannot create workqueue\n");
        goto r_device;
    }

    // Initialize work structure
    INIT_WORK(&my_work, workqueue_fn);

    pr_info("Device Driver Inserted\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, 1);
    cdev_del(&etx_cdev);
    return -1;
}

// Module Exit
static void __exit etx_driver_exit(void) {
    flush_workqueue(workqueue);
    destroy_workqueue(workqueue);
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Removed\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);
