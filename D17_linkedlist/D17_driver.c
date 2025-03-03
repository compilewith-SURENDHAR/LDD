#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("surendhar");
MODULE_DESCRIPTION("Kernel Linked List in Device Driver");

// Define a structure to store data with the linked list
struct my_node {
    int data;
    struct list_head list;  // Embedded linked list
};

// Declare and initialize a linked list head
LIST_HEAD(my_list);

/* Function to add elements to the list */
static void add_elements(void) {
    int i;
    struct my_node *new_node;

    for (i = 1; i <= 5; i++) {
        new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        if (!new_node) {
            pr_err("Memory allocation failed\n");
            return;
        }
        new_node->data = i * 10;
        list_add_tail(&new_node->list, &my_list);
        pr_info("Added: %d\n", new_node->data);
    }
}

/* Function to traverse and print the list */
static void traverse_list(void) {
    struct my_node *node;
    pr_info("Traversing linked list:\n");
    list_for_each_entry(node, &my_list, list) {
        pr_info("Data: %d\n", node->data);
    }
}

/* Function to delete a specific element (e.g., data = 30) */
static void delete_element(int target) {
    struct my_node *node, *tmp;
    list_for_each_entry_safe(node, tmp, &my_list, list) {
        if (node->data == target) {
            pr_info("Deleting: %d\n", node->data);
            list_del(&node->list);
            kfree(node);
            return;
        }
    }
    pr_info("Element %d not found\n", target);
}

/* Function to check if the list is empty */
static void check_empty(void) {
    if (list_empty(&my_list)) {
        pr_info("List is empty\n");
    } else {
        pr_info("List is NOT empty\n");
    }
}

/* Module Init: Runs when the module is inserted */
static int __init linked_list_init(void) {
    pr_info("Kernel Linked List Module Loaded\n");
    
    add_elements();
    traverse_list();
    delete_element(30);
    traverse_list();
    check_empty();

    return 0;
}

/* Module Exit: Runs when the module is removed */
static void __exit linked_list_exit(void) {
    struct my_node *node, *tmp;
    
    // Delete all nodes before exiting
    list_for_each_entry_safe(node, tmp, &my_list, list) {
        pr_info("Freeing: %d\n", node->data);
        list_del(&node->list);
        kfree(node);
    }
    
    pr_info("Kernel Linked List Module Unloaded\n");
}

module_init(linked_list_init);
module_exit(linked_list_exit);
