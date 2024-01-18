
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

static struct net_device *my_net_device;

static int my_hard_start_xmit(struct sk_buff *skb, struct net_device *dev) {
    // Здесь вы можете имитировать отправку пакета
    pr_info("Пакет отправлен\n");
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static int my_open(struct net_device *dev) {
    netif_start_queue(dev);
    return 0;
}

static int my_close(struct net_device *dev) {
    netif_stop_queue(dev);
    return 0;
}

static const struct net_device_ops my_netdev_ops = {
    .ndo_open = my_open,
    .ndo_stop = my_close,
    .ndo_start_xmit = my_hard_start_xmit,
};

static void my_setup(struct net_device *dev) {
    dev->netdev_ops = &my_netdev_ops;
    eth_hw_addr_random(dev);
    dev->flags |= IFF_NOARP;
    dev->features |= NETIF_F_HW_CSUM;
}

static int __init my_init(void) {
    my_net_device = alloc_netdev(0, "mydev%d", NET_NAME_UNKNOWN, my_setup);
    if (register_netdev(my_net_device)) {
        pr_err("Не удалось зарегистрировать сетевой интерфейс\n");
        return -ENOMEM;
    }
    return 0;
}

static void __exit my_exit(void) {
    unregister_netdev(my_net_device);
    free_netdev(my_net_device);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Ваше Имя");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Пример сетевого драйвера");
