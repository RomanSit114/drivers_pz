#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>


MODULE_LICENSE("GPL");


irqreturn_t irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
  static unsigned char scancode;
  unsigned char status;

  status = inb(0x64);
  scancode = inb(0x60);

switch (scancode)
{
  case 0x01:  printk (KERN_INFO "test1 ...\n");
              break;
  case 0x3B:  printk (KERN_INFO "test2 ...\n");
              break;
  case 0x3C:  printk (KERN_INFO "test3 ...\n");
              break;
  default:
              break;
}

  return IRQ_HANDLED;
}

/
static int __init irq_ex_init(void)
{
    free_irq(1,NULL);
    return request_irq (1, (irq_handler_t) irq_handler,IRQF_SHARED, "test_keyboard_irq_handler",(void *)(irq_handler));
}

static void __exit irq_ex_exit(void)
{
    free_irq(1,(void*)irq_handler);
}

module_init(irq_ex_init);
module_exit(irq_ex_exit);
