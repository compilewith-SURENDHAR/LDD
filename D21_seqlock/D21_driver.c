#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/seqlock.h>

/* Global Variables */
static struct task_struct *writer_thread, *reader_thread;
static int shared_counter = 0;  // Shared variable protected by seqlock

seqlock_t my_seqlock;  // Define SeqLock

/* Writer Thread Function */
static int writer_function(void *data) {
    while (!kthread_should_stop()) {
        write_seqlock(&my_seqlock);  // Acquire exclusive write lock
        shared_counter++;            // Modify shared variable
        pr_info("Writer: Updated counter = %d\n", shared_counter);
        write_sequnlock(&my_seqlock);  // Release lock

        msleep(1000);  // Simulate work (1 sec delay)
    }
    return 0;
}

/* Reader Thread Function */
static int reader_function(void *data) {
    int read_value, seq;

    while (!kthread_should_stop()) {
        do {
            seq = read_seqbegin(&my_seqlock);  // Begin reading
            read_value = shared_counter;       // Read shared variable
        } while (read_seqretry(&my_seqlock, seq));  // Retry if updated during read

        pr_info("Reader: Read counter = %d\n", read_value);
        msleep(500);  // Simulate work (500 ms delay)
    }
    return 0;
}

/* Module Initialization */
static int __init seqlock_example_init(void) {
    seqlock_init(&my_seqlock);  // Initialize SeqLock

    /* Create Kernel Threads */
    writer_thread = kthread_run(writer_function, NULL, "writer_thread");
    reader_thread = kthread_run(reader_function, NULL, "reader_thread");

    if (IS_ERR(writer_thread) || IS_ERR(reader_thread)) {
        pr_err("Failed to create threads\n");
        return -1;
    }

    pr_info("SeqLock Example Module Loaded!\n");
    return 0;
}

/* Module Cleanup */
static void __exit seqlock_example_exit(void) {
    if (writer_thread)
        kthread_stop(writer_thread);
    if (reader_thread)
        kthread_stop(reader_thread);

    pr_info("SeqLock Example Module Unloaded!\n");
}

module_init(seqlock_example_init);
module_exit(seqlock_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surendhar");
MODULE_DESCRIPTION("Linux Kernel SeqLock ");
