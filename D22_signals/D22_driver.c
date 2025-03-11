#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>         
#include <linux/uaccess.h>      
#include <linux/ioctl.h>
#include <linux/sched/signal.h> 

#define SIGETX 44
#define REG_CURRENT_TASK _IOW('a', 'a', int32_t*)

static struct task_struct *task = NULL;
static int signum = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .write          = etx_write,
    .open           = etx_open,
    .unlocked_ioctl = etx_ioctl,
    .release        = etx_release,
};

static int etx_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device File Opened\n");
    return 0;
}

static int etx_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device File Closed\n");
    return 0;
}

// **SIGNAL SENT FROM WRITE OPERATION**
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
    struct kernel_siginfo info;

    printk(KERN_INFO "Write function: Sending signal to user-space\n");

    memset(&info, 0, sizeof(struct kernel_siginfo));
    info.si_signo = SIGETX;
    info.si_code = SI_QUEUE;
    info.si_int = 1;

    if (task != NULL) {
        if (send_sig_info(SIGETX, &info, task) < 0) {
            printk(KERN_INFO "Failed to send signal\n");
        } else {
            printk(KERN_INFO "Signal sent successfully\n");
        }
    }

    return len;
}

// **USER-SPACE REGISTERS ITS TASK USING IOCTL**
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    if (cmd == REG_CURRENT_TASK) {
        task = get_current(); // Store the user-space process
        signum = SIGETX;
        printk(KERN_INFO "User process registered for signals\n");
    }
    return 0;
}

static int __init etx_driver_init(void) {
    if ((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) < 0) {
        printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
    }

    printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    cdev_init(&etx_cdev, &fops);
    if ((cdev_add(&etx_cdev, dev, 1)) < 0) {
        printk(KERN_INFO "Cannot add device\n");
        goto r_class;
    }

    if (IS_ERR(dev_class = class_create( "etx_class"))) {
        printk(KERN_INFO "Cannot create class\n");
        goto r_class;
    }

    if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "etx_device"))) {
        printk(KERN_INFO "Cannot create device\n");
        goto r_device;
    }

    printk(KERN_INFO "Device Driver Inserted Successfully\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, 1);
    return -1;
}

static void __exit etx_driver_exit(void) {
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Removed\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("Send signal from kernel to user-space");
