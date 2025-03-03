#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>    
#include <linux/uaccess.h> 
#include <linux/kthread.h> 
#include <linux/sched.h>   
#include <linux/delay.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("A Multi-Threaded Kernel Module");
MODULE_VERSION("1.0");

int thread_function1(void *pv);
int thread_function2(void *pv);

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static struct task_struct *thread1;
static struct task_struct *thread2;

// Thread Function 1
int thread_function1(void *pv)
{
    int i = 0;
    while (!kthread_should_stop())
    {
        pr_info("Thread 1: Count = %d\n", i++);
        msleep(1000); // Sleep for 1 second
    }
    return 0;
}

// Thread Function 2
int thread_function2(void *pv)
{
    int i = 0;
    while (!kthread_should_stop())
    {
        pr_info("Thread 2: Count = %d\n", i++);
        msleep(2000); // Sleep for 2 seconds
    }
    return 0;
}

// Module Init Function
static int __init etx_driver_init(void)
{
    // Allocating Major Number
    if ((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) < 0)
    {
        pr_err("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    // Creating cdev structure
    cdev_init(&etx_cdev, NULL);

    // Adding Character Device
    if ((cdev_add(&etx_cdev, dev, 1)) < 0)
    {
        pr_err("Cannot add the device to the system\n");
        goto r_class;
    }

    // Creating struct class
    if (IS_ERR(dev_class = class_create("etx_class")))
    {
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }

    // Creating device
    if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "etx_device")))
    {
        pr_err("Cannot create the Device \n");
        goto r_device;
    }

    // Creating and Starting Thread 1
    thread1 = kthread_create(thread_function1, NULL, "Thread_1");
    if (thread1)
    {
        wake_up_process(thread1);
    }
    else
    {
        pr_err("Cannot create thread 1\n");
        goto r_device;
    }

    // Creating and Starting Thread 2
    thread2 = kthread_create(thread_function2, NULL, "Thread_2");
    if (thread2)
    {
        wake_up_process(thread2);
    }
    else
    {
        pr_err("Cannot create thread 2\n");
        goto r_device;
    }

    pr_info("Multi-Threaded Device Driver Inserted...Done!!!\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, 1);
    cdev_del(&etx_cdev);
    return -1;
}

// Module Exit Function
static void __exit etx_driver_exit(void)
{
    kthread_stop(thread1);
    kthread_stop(thread2);
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Multi-Threaded Device Driver Removed...Done!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);
