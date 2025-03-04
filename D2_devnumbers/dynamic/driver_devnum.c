#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("Simple linux driver for Dynamically allocating device number)");
MODULE_VERSION("1.0");
 
dev_t dev = 0;

static int __init devnum_init(void)
{
        if((alloc_chrdev_region(&dev, 0, 1, "dev_num1")) <0){
                printk(KERN_INFO "Cannot allocate major number for device 1\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        printk(KERN_INFO "Kernel Module Inserted Successfully!\n");
        
        return 0;
}


static void __exit devnum_exit(void)
{
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "Kernel Module Removed Successfully!\n");
}
 
 
module_init(devnum_init);
module_exit(devnum_exit);
 

