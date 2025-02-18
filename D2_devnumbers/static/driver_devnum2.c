#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("surendhar");
MODULE_DESCRIPTION("linux driver Statically allocating the Device number)");
MODULE_VERSION("1.0");

//creating the dev with our custom major and minor number
dev_t dev = MKDEV(265, 0);

static int __init static_init(void)
{
    register_chrdev_region(dev, 1, "devnum2");
    printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
    printk(KERN_INFO "Kernel Module Inserted Successfully!\n");
    return 0;
}

static void __exit static_exit(void)
{
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Kernel Module Removed Successfully!\n");
}
 
module_init(static_init);
module_exit(static_exit);
 

