#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");

static struct task_struct *my_thread; // Переменная для потока ядра
static DEFINE_MUTEX(my_mutex); // Мьютекс для безопасности доступа к переменной

static int shared_variable = 0; // Общая переменная для чтения и записи

// Функция потока ядра
static int my_thread_function(void *data) {
    while (!kthread_should_stop()) {
        pr_info("My kernel thread is running!\n"); // Вывод сообщения о работе потока
        msleep(1000); // Приостановка на 1 секунду
    }

    pr_info("My kernel thread is stopping!\n"); // Сообщение о завершении работы потока
    return 0;
}

// Открытие устройства
static int my_device_open(struct inode *inode, struct file *file) {
    pr_info("Device file opened.\n"); // Сообщение об открытии файла устройства
    return 0;
}

// Закрытие устройства
static int my_device_release(struct inode *inode, struct file *file) {
    pr_info("Device file closed.\n"); // Сообщение о закрытии файла устройства
    return 0;
}

// Чтение данных из устройства в пользовательский буфер
static ssize_t my_device_read(struct file *file, char __user *user_buffer, size_t count, loff_t *ppos) {
    int bytes_to_copy;
    mutex_lock(&my_mutex); // Захват мьютекса для безопасного доступа

    if (*ppos > 0) {
        mutex_unlock(&my_mutex);
        return 0; // Все данные уже прочитаны
    }

    bytes_to_copy = min(count, sizeof(int));
    if (copy_to_user(user_buffer, &shared_variable, bytes_to_copy) != 0) {
        mutex_unlock(&my_mutex);
        return -EFAULT; // Ошибка копирования данных в пользовательский буфер
    }

    *ppos += bytes_to_copy;
    mutex_unlock(&my_mutex);

    return bytes_to_copy;
}

// Запись данных из пользовательского буфера в устройство
static ssize_t my_device_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {
    int bytes_to_copy;
    mutex_lock(&my_mutex); // Захват мьютекса для безопасного доступа

    if (*ppos > 0) {
        mutex_unlock(&my_mutex);
        return -ENOSPC; // Превышена емкость устройства
    }

    bytes_to_copy = min(count, sizeof(int));
    if (copy_from_user(&shared_variable, user_buffer, bytes_to_copy) != 0) {
        mutex_unlock(&my_mutex);
        return -EFAULT; // Ошибка копирования данных из пользовательского буфера
    }

    *ppos += bytes_to_copy;
    mutex_unlock(&my_mutex);

    return bytes_to_copy;
}

// Определение операций устройства
static struct file_operations fops = {
    .open = my_device_open,
    .release = my_device_release,
    .read = my_device_read,
    .write = my_device_write,
};

// Инициализация модуля
static int __init my_module_init(void) {
    pr_info("My module is being loaded.\n"); // Сообщение о загрузке модуля

    // Создание потока ядра
    my_thread = kthread_run(my_thread_function, NULL, "my_kernel_thread");
    if (my_thread) {
        pr_info("My kernel thread is started.\n"); // Сообщение о запуске потока
    } else {
        pr_err("Failed to create kernel thread.\n"); // Сообщение об ошибке создания потока
        return -ENOMEM;
    }

    // Регистрация символьного устройства
    if (register_chrdev(0, "roma_device", &fops) < 0) {
        pr_err("Failed to register character device.\n"); // Сообщение об ошибке регистрации устройства
        return -EBUSY;
    }

    return 0;
}

// Выход из модуля
static void __exit my_module_exit(void) {
    pr_info("My module is being unloaded.\n"); // Сообщение о выгрузке модуля

    if (my_thread) {
        kthread_stop(my_thread);
        pr_info("My kernel thread is stopped.\n"); // Сообщение об остановке потока
    }

    unregister_chrdev(0, "roma_device"); // Удаление зарегистрированного устройства
}

module_init(my_module_init);
module_exit(my_module_exit);


