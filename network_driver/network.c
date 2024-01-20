#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/etherdevice.h>
#include <linux/kernel.h>

#define INITIAL_VALUE 10 // Начальное значение для поля some_field

typedef struct { 
   int some_field;
   unsigned long trans_start; // поле для отслеживания времени начала передачи 
} MY_DRIVERDATA; 

struct net_device *netdev;

int my_open(struct net_device *dev) {
   /* Установка MAC-адреса устройства */
    unsigned char mac_addr[ETH_ALEN] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}; // MAC-адрес
    MY_DRIVERDATA *data = netdev_priv(dev); // Получение доступа к приватным данным устройства

    data->some_field = INITIAL_VALUE; // Инициализация полей структуры

    memcpy((void *)dev->dev_addr, mac_addr, ETH_ALEN); // Копирование MAC-адреса в dev_addr

    netif_start_queue(dev); // Запуск очереди устройства для передачи данных
    return 0;
}


int my_close(struct net_device *dev) {
    netif_stop_queue(dev); // Остановка очереди устройства для передачи данных
    return 0;
}

int my_set_mac_address(struct net_device *dev, void *addr) {
    if (!is_valid_ether_addr(addr)) {
        return -EADDRNOTAVAIL; // Возвращается ошибка, если MAC-адрес недопустим
    }

    /* Установка нового MAC-адреса для устройства */
    memcpy((void *)dev->dev_addr, addr, ETH_ALEN);

    return 0;
}


int my_hard_start_xmit( struct sk_buff *skb, struct net_device *dev )
{
/* Инициализация передачи апаратурой */
//dev->trans_start = jiffies;
dev->stats.tx_packets += 1;
dev->stats.tx_bytes += skb->len;

dev_kfree_skb( skb );// Освобождение skb, так как мы не отправляем реальные пакеты

return NETDEV_TX_OK; // Успешное завершение передачи пакета
}

void my_rxhandler(unsigned long data) {
    struct net_device *dev = (struct net_device *)data;
    struct sk_buff *skb;
    int rxbytes = 60; // пример размера пакета

    skb = dev_alloc_skb(rxbytes + 2);
    if (!skb) {
       pr_err("Failed to allocate skb memory\n");
       return; // Выход из функции в случае неудачи
    }

    skb->dev = dev;
    skb->protocol = eth_type_trans(skb, dev);
    skb->ip_summed = CHECKSUM_NONE;

    /* Обновление статистики rx_packets и rx_bytes */
    dev->stats.rx_packets += 1;
    dev->stats.rx_bytes += rxbytes;

    /* Печать информационного сообщения */
    pr_info("Received a packet! rx_packets: %lu, rx_bytes: %lu\n", dev->stats.rx_packets, dev->stats.rx_bytes);

    dev_kfree_skb(skb); // Освобождение skb после обработки
}


static const struct net_device_ops my_netdev_ops = {
   .ndo_open = my_open,
   .ndo_stop = my_close,
   .ndo_set_mac_address = my_set_mac_address,
   .ndo_start_xmit = my_hard_start_xmit,
} ;

int my_init_module( void ) {
    //unsigned char new_mac_addr[ETH_ALEN] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}; // Новый MAC-адрес

    netdev = alloc_etherdev(sizeof(MY_DRIVERDATA));
    if (!netdev) return -ENOMEM;

    netdev->netdev_ops = &my_netdev_ops;

    if (netdev->trans_start)
        dev->trans_start = jiffies; // Присваивание значения 'jiffies' полю 'trans_start' структуры 'net_device'*/

    return register_netdev(netdev);
}

// module_exit
static void __exit my_exit( void ) {
    if (netdev) {
    unregister_netdev(netdev); // Снятие регистрации устройства
    free_netdev(netdev); // Освобождение структуры net_device
    netdev = NULL; // Установка указателя netdev в NULL
    }
}

module_init(my_init_module);
module_exit(my_exit);

MODULE_LICENSE("GPL");
