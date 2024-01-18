#include <linux/netdevice.h> 
#include <linux/skbuff.h> 
#include <linux/etherdevice.h> 
#include <linux/printk.h> 
 
typedef struct { 
    // Данные драйвера 
} NET_DRIVERDATA; 
 
static int eth_open(struct net_device *dev); 
static int eth_stop(struct net_device *dev); 
static int eth_start_xmit(struct sk_buff *skb, struct net_device *dev); 
static int eth_receive(struct sk_buff *skb, struct net_device *dev); 
 
static const struct net_device_ops eth_netdev_ops = { 
    .ndo_open = eth_open, 
    .ndo_stop = eth_stop, 
    .ndo_start_xmit = eth_start_xmit, 
   
}; 
 
static int eth_open(struct net_device *dev) { 
    netif_start_queue(dev); 
    return 0; 
} 
 
static int eth_stop(struct net_device *dev) { 
    netif_stop_queue(dev); 
    return 0; 
} 
 
static int eth_start_xmit(struct sk_buff *skb, struct net_device *dev) { 
    pr_info("Hello start xmit\n")
    dev_kfree_skb(skb); 
    dev->stats.tx_packets++; 
    dev->stats.tx_bytes += skb->len; 
    return NETDEV_TX_OK; 
} 
 
static int eth_receive(struct sk_buff *skb, struct net_device *dev) { 
    pr_info("Packet received on %s\n", dev->name); 
    // Обработка пакета 
    return NET_RX_SUCCESS; 
} 
 
struct net_device *netdev; 
 
int init_module(void) { 
    pr_info("Hello\n");
    netdev = alloc_netdev(sizeof(NET_DRIVERDATA), "eth%d", NET_NAME_UNKNOWN, ether_setup); 
    if (!netdev) 
        return -ENOMEM; 
 
    netdev->netdev_ops = &eth_netdev_ops; 
    return register_netdev(netdev); 
} 
 
void cleanup_module(void) { 
    if (netdev) { 
        unregister_netdev(netdev); 
        free_netdev(netdev); 
    } 
} 
 
MODULE_LICENSE("GPL");

