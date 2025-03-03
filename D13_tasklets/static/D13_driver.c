
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include<linux/sysfs.h> 
#include<linux/kobject.h> 
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/err.h>
 
#define IRQ_NO 1
 
void tasklet_fn(struct tasklet_struct *tasklet);


/* Init the Tasklet by Static Method */
DECLARE_TASKLET(tasklet,tasklet_fn);
 
/*Tasklet Function*/
void tasklet_fn(struct tasklet_struct *tasklet)
{
        printk(KERN_INFO "Executing Tasklet Function : \n" );
}
 
 
//Interrupt handler for IRQ 1 
static irqreturn_t irq_handler(int irq,void *dev_id) {

          // Mark as handled
        printk(KERN_INFO "Keyboard Interrupt Occurred! IRQ: %d\n", irq);

        // Schedule the tasklet
        tasklet_schedule(&tasklet);

        return IRQ_HANDLED;
}
 
volatile int etx_value = 0;
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
struct kobject *kobj_ref;

 
/*************** Driver Functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
 
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = etx_read,
        .write          = etx_write,
        .open           = etx_open,
        .release        = etx_release,
};
 
    
static int etx_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Opened...!!!\n");
        return 0;
}

   
static int etx_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Closed...!!!\n");
        return 0;
}


static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Read function\n");
        return 0;
}


static ssize_t etx_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Write Function\n");
        return len;
}
 
 
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create("etx_class"))){
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device"))){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto r_device;
        }
 
        printk(KERN_INFO "Registering Keyboard Interrupt Handler...\n");

        if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "etx_keyboard_irq", (void *)(irq_handler))) {
                printk(KERN_ERR "Cannot register keyboard IRQ 1\n");
                return -1;
        }

        printk(KERN_INFO "Keyboard Interrupt Registered Successfully on IRQ %d\n", IRQ_NO);
        return 0;
        
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        cdev_del(&etx_cdev);   
        return -1;
}


static void __exit etx_driver_exit(void)
{
        /*Kill the Tasklet */ 
        printk(KERN_INFO "Unregistering Keyboard Interrupt Handler...\n");
        tasklet_kill(&tasklet);
        free_irq(IRQ_NO,(void *)(irq_handler));
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("surendhar");
MODULE_DESCRIPTION("A simple device driver - Tasklet Static");
MODULE_VERSION("1.0");