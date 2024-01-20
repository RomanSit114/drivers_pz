// vendor id - 8086
// device -d - 100f
// MAC адрес: 00:0C:29:B0:39:A8

#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "pci_mac"
#define MAC_ADDR_LENGTH 6

static dev_t dev_num;
static struct cdev *char_device = NULL;
static struct pci_dev *pci_device = NULL;

// Открытие файла устройства
static int pci_mac_open(struct inode *inode, struct file *file) {
    return 0;
}

// Закрытие файла устройства
static int pci_mac_release(struct inode *inode, struct file *file) {
    return 0;
}

// функция для обработки IOCTL вызовов
static long pci_mac_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    unsigned char mac_addr[MAC_ADDR_LENGTH];

    if (cmd != 0) {
        return -EINVAL;
    }

    if (!pci_device) {
        printk(KERN_ERR "PCI device not found\n");
        return -ENODEV;
    }

    // Получение MAC-адреса из PCI устройства

    // Получение регистра BAR (Base Address Register)
    int bar = pci_select_bars(pci_device, IORESOURCE_MEM);
    if (bar == 0) {
        printk(KERN_ERR "Failed to select BAR\n");
        return -EFAULT;
    }

    // Получение базового адреса BAR
    resource_size_t bar_start = pci_resource_start(pci_device, bar);
    if (!bar_start) {
        printk(KERN_ERR "Failed to get BAR start address\n");
        return -EFAULT;
    }

    unsigned int *mac_ptr = ioremap(bar_start, MAC_ADDR_LENGTH);
    if (!mac_ptr) {
        printk(KERN_ERR "Failed to map BAR memory\n");
        return -EFAULT;
    }

    // Копирование MAC-адреса из памяти устройства
    memcpy(mac_addr, mac_ptr, MAC_ADDR_LENGTH);
    iounmap(mac_ptr);

    // Копирование MAC-адреса в буфер пользователя
    if (copy_to_user((unsigned char *)arg, mac_addr, MAC_ADDR_LENGTH) != 0) {
        return -EFAULT;
    }

    return 0;
}

// Структура операций с файлами для устройства
static struct file_operations fops = {
    .open = pci_mac_open,
    .release = pci_mac_release,
    .unlocked_ioctl = pci_mac_ioctl,
};

// Инициализация модуля
static int __init pci_mac_init(void) {
    // Регистрация номера устройства
    int ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate character device\n");
        return ret;
    }

    // Выделение памяти для структуры устройства
    char_device = cdev_alloc();
    if (!char_device) {
        printk(KERN_ERR "Failed to allocate cdev\n");
        unregister_chrdev_region(dev_num, 1);
        return -ENOMEM;
    }

    // Инициализация структуры устройства
    cdev_init(char_device, &fops);
    ret = cdev_add(char_device, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "Failed to add character device\n");
        cdev_del(char_device);
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    // Поиск PCI устройства по Vendor и Device ID
    pci_device = pci_get_device(0x8086, 0x100f, NULL);
    if (!pci_device) {
        printk(KERN_ERR "PCI device not found\n");
        cdev_del(char_device);
        unregister_chrdev_region(dev_num, 1);
        return -ENODEV;
    }

    printk(KERN_INFO "PCI MAC driver initialized\n");
    return 0;
}

// Выход из модуля
static void __exit pci_mac_exit(void) {
    if (pci_device) {
        pci_dev_put(pci_device);
    }

    cdev_del(char_device);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "PCI MAC driver exited\n");
}

module_init(pci_mac_init);
module_exit(pci_mac_exit);

MODULE_LICENSE("GPL");

