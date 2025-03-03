#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>

#define TIMER_INTERVAL 5000  // 5000ms = 5 seconds

static struct timer_list my_timer;
void timer_callback(struct timer_list *timer);

// Callback function when timer expires
void timer_callback(struct timer_list *timer)
{
    pr_info("Kernel Timer Fired! Restarting timer...\n");

    // Re-schedule the timer to fire again in 5 seconds
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL));
}

static int __init timer_init(void)
{
    pr_info("Kernel Timer Module Loaded\n");

    // Initialize timer
    timer_setup(&my_timer, timer_callback, 0);

    // Schedule the timer to run after 5 seconds
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL));

    return 0;
}

static void __exit timer_exit(void)
{
    // Delete the timer before unloading the module
    del_timer_sync(&my_timer);
    pr_info("Kernel Timer Module Unloaded\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("surendhar");
MODULE_DESCRIPTION("A simple Linux Kernel Timer");
