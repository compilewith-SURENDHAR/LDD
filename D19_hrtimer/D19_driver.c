#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define TIMER_INTERVAL_MS 200  // Set timer interval to 200ms

static struct hrtimer my_hrtimer;
static ktime_t kt;
static ktime_t last_time;

static enum hrtimer_restart my_timer_callback(struct hrtimer *timer)
{
    ktime_t now = ktime_get();

    if (!ktime_to_ns(last_time))
        last_time = now;  // Initialize first call
    
    pr_info("Time since last execution: %lld ns\n", ktime_to_ns(ktime_sub(now, last_time)));

    last_time = now;  // Update last execution time

    // Restart timer with a precise 200ms interval
    hrtimer_forward_now(timer, kt);
    return HRTIMER_RESTART;
}

static int __init my_hrtimer_init(void)
{
    pr_info("Initializing High-Resolution Timer Module\n");

    // Set the timer interval
    kt = ktime_set(0, TIMER_INTERVAL_MS * 1000000);  // 200ms in nanoseconds

    // Initialize the timer
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_hrtimer.function = my_timer_callback;

    // Start the timer
    hrtimer_start(&my_hrtimer, kt, HRTIMER_MODE_REL);

    return 0;
}

static void __exit my_hrtimer_exit(void)
{
    pr_info("Exiting High-Resolution Timer Module\n");

    // Cancel the timer
    hrtimer_cancel(&my_hrtimer);
}

module_init(my_hrtimer_init);
module_exit(my_hrtimer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("surendhar");
MODULE_DESCRIPTION("High-Resolution Timer Example with 200ms Interval");
