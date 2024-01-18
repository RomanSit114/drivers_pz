#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/etherdevice.h>


struct net_device {
char name[IFNAMSIZ];
// I/O specific fields
unsigned long mem_end; /* shared mem end */
unsigned long mem_start;/* shared mem start */
unsigned long base_addr;/* device I/O address */
unsigned int irq; /* device IRQ number */

const struct net_device_ops *netdev_ops; // Management operations 
}


struct net_device_ops {
int (*ndo_init)(struct net_device *dev);
void (*ndo_uninit)(struct net_device *dev);
int (*ndo_open)(struct net_device *dev);
int (*ndo_stop)(struct net_device *dev);
netdev_tx_t (*ndo_start_xmit) (struct sk_buff *skb, struct net_device *dev);
u16 (*ndo_select_queue)(struct net_device *dev, struct sk_buff *skb);
}

// module init
typedef struct { /* внутренние данные драйвера */ } MY_DRIVERDATA;
struct net_device *netdev;
static const struct net_device_ops my_netdev_ops = {
	.ndo_open = my_open,
	.ndo_stop = my_close,
	.ndo_set_mac_address = my_set_mac_address,
	.ndo_start_xmit = my_start_xmit,
} ;
static int init_module( void ) {
netdev = alloc_etherdev( sizeof( MY_DRIVERDATA ) );
if ( !netdev ) return -ENOMEM;
netdev->netdev_ops = &my_netdev_ops;
return register_netdev( netdev );
}


// module exit
struct net_device *netdev;
static void __exit my_exit( void ) {
unregister_netdev( netdev );
free_netdev( netdev );
}
