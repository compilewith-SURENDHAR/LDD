#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
 
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);

int etx_count = 0;              //Exported variable
void etx_shared_func(void);

 
void etx_shared_func(void)
{
        pr_info("Shared function been called!!!\n");
        etx_count++;
}

//EXPORT_SYMBOL_GPL(etx_shared_func);
EXPORT_SYMBOL(etx_shared_func);
EXPORT_SYMBOL(etx_count);
 
static ssize_t etx_read(struct file *filp, 
    char __user *buf, size_t len, loff_t *off)
{
pr_info("Data Read : Done!\n");
return 1;
}

static ssize_t etx_write(struct file *filp, 
    const char __user *buf, size_t len, loff_t *off)
{
pr_info("Data Write : Done!\n");
return len;
}

static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = etx_read,
        .write          = etx_write,
};

static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev1")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        

        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);

        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create("etx_class1"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device1"))){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("Device Driver 1 Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}


static void __exit etx_driver_exit(void)
{
        device_destroy(dev_class,dev);
        cdev_del(&etx_cdev);
        class_destroy(dev_class);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver 1 Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("surendhar");
MODULE_DESCRIPTION("EXPORT_SYMBOL Driver - 1");
MODULE_VERSION("1.25");