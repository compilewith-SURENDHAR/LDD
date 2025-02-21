#include <linux/module.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "waitqueue_device"
#define CLASS_NAME "waitqueue_class"

static char device_buffer[80];  // Buffer for data
static int data_available = 0;  // Flag to check if data is available
static wait_queue_head_t my_waitqueue;  // Declare wait queue

static dev_t dev;  // Device number
static struct cdev my_cdev;  // Character device structure
static struct class *dev_class;  // Device class



/* Read Function (Blocking Read) */
static ssize_t my_read(struct file *file, char __user *user_buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "Read Operation: Waiting for data...\n");

    // Wait until data is available (blocking)
    wait_event_interruptible(my_waitqueue, data_available != 0);

    if (copy_to_user(user_buffer, device_buffer, len)) {
        return -EFAULT;
    }

    printk(KERN_INFO "Read Operation: Data Sent to User\n");

    data_available = 0;  // Reset flag after reading
    return len;
}

/* Write Function (Wakes Up Reader) */
static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *offset) {
    if (copy_from_user(device_buffer, user_buffer, len)) {
        return -EFAULT;
    }

    printk(KERN_INFO "Write Operation: Data Received, Waking up Readers\n");
    
    data_available = 1;  // Set flag indicating data is available
    wake_up_interruptible(&my_waitqueue);  // Wake up any waiting processes

    return len;
}


/* File Operations */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};


/* Module Initialization */
static int __init D7_driver_init(void) {
    int ret;

    // Allocate Major and Minor numbers dynamically
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate device number\n");
        return ret;
    }

    // Initialize and add character device
    cdev_init(&my_cdev, &fops);
    ret = cdev_add(&my_cdev, dev, 1);
    if (ret < 0) {
        printk(KERN_ERR "Failed to add cdev\n");
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    // Create class for device
    dev_class = class_create(CLASS_NAME);
    if (IS_ERR(dev_class)) {
        printk(KERN_ERR "Failed to create class\n");
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(dev_class);
    }

    // Create device file in /dev
    if (IS_ERR(device_create(dev_class, NULL, dev, NULL, DEVICE_NAME))) {
        printk(KERN_ERR "Failed to create device file\n");
        class_destroy(dev_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    // Initialize wait queue
    init_waitqueue_head(&my_waitqueue);

    printk(KERN_INFO "Wait Queue Driver Loaded\n");
    return 0;
}


static void __exit D7_driver_exit(void) {
    device_destroy(dev_class, dev);  // Remove device file
    class_destroy(dev_class);  // Destroy class
    cdev_del(&my_cdev);  // Remove cdev
    unregister_chrdev_region(dev, 1);  // Free device number

    printk(KERN_INFO "Wait Queue Driver Unloaded\n");
}


module_init(D7_driver_init);
module_exit(D7_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("Linux Character Driver using Wait Queues");
