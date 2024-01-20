#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "hello"
#define BUFFER_SIZE 32

static char hello_buffer[BUFFER_SIZE] = "Hello";
static int major_number;

static int hello_open(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t hello_read(struct file *file, char *buf, size_t count, loff_t *offset)
{
    ssize_t bytes_read = 0;
    if (*offset == 0) {
        bytes_read = simple_read_from_buffer(buf, count, offset, hello_buffer, BUFFER_SIZE);
    }
    return bytes_read;
}

static struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .read = hello_read,
};

static int __init hello_init(void)
{
    major_number = register_chrdev(0, DRIVER_NAME, &hello_fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "Registered a major number %d\n", major_number);
    return 0;
}

static void __exit hello_exit(void)
{
    unregister_chrdev(major_number, DRIVER_NAME);
    printk(KERN_INFO "Unregistered the major number\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");

