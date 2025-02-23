#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("Keyboard Interrupt Counter using Sysfs");



static int irq = 1;  // IRQ number for the keyboard (IRQ 1)
static unsigned int key_count = 0;  // Interrupt counter
static struct kobject *kobj_ref;

// Interrupt Handler Function
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id) {
    key_count++;  // Increment the counter on each key press
    pr_info("Keyboard Interrupt Occurred! Count = %d\n", key_count);
    return IRQ_HANDLED;
}

// Sysfs Show Function
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", key_count);
}

// Creating sysfs attribute
static struct kobj_attribute key_count_attr = __ATTR(key_count, 0444, sysfs_show, NULL);

// Module Initialization
static int __init keyboard_irq_init(void) {
    if (request_irq(irq, keyboard_irq_handler, IRQF_SHARED, "keyboard_irq", (void *)(keyboard_irq_handler))) {
        pr_err("Cannot register keyboard interrupt\n");
        return -1;
    }

    // Create sysfs entry
    kobj_ref = kobject_create_and_add("keyboard_counter", kernel_kobj);
    if (sysfs_create_file(kobj_ref, &key_count_attr.attr)) {
        pr_err("Cannot create sysfs file\n");
        kobject_put(kobj_ref);
        free_irq(irq, (void *)(keyboard_irq_handler));
        return -1;
    }

    pr_info("Keyboard Interrupt Module Loaded!\n");
    return 0;
}

// Module Exit
static void __exit keyboard_irq_exit(void) {
    free_irq(irq, (void *)(keyboard_irq_handler));
    sysfs_remove_file(kobj_ref, &key_count_attr.attr);
    kobject_put(kobj_ref);
    pr_info("Keyboard Interrupt Module Unloaded!\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

