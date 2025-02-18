#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("the drive code implementing the parameter passing");
MODULE_VERSION("1.0");


int numVal, arrVal[4];
char *nameVal;
//int cbVal = 0;

module_param(numVal, int , S_IRUSR|S_IWUSR);
module_param(nameVal, charp , S_IRUSR|S_IWUSR);
module_param_array(arrVal, int, NULL, S_IRUSR|S_IWUSR);


static int __init hello_driver_init(void)
{
printk(KERN_INFO "this is the first driver module!");
printk(KERN_INFO "our number : %d \n", numVal);
printk(KERN_INFO "the name : %s \n", nameVal);
for( int i=0; i < (sizeof(arrVal)/sizeof(int)); i++)
{
printk(KERN_INFO "array[%d] = %d \n", i, arrVal[i]);
}
printk(KERN_INFO "kernel module inserted! \n");
return 0;
}


static void __exit hello_driver_exit(void)
{
printk(KERN_INFO "kernel module removed successfully! \n");
}


module_init(hello_driver_init);
module_exit(hello_driver_exit);


