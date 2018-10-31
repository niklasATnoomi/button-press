
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/gpio.h>
#include <mach/iomux-v3.h>
#include <mach/iomux-mx6dl.h>
 
#define DEV_NAME        "pmu"
#define VPU_POW_INT_PIN IMX_GPIO_NR(0, 24)
#define VPU_POW_INT     gpio_to_irq(VPU_POW_INT_PIN)
#define EVENT_POW_OFF   "EVENT_CHARGE_OFF\n"
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("king");
MODULE_DESCRIPTION("ywwh vpu pow detect");
 
static DECLARE_WAIT_QUEUE_HEAD(vpu_pow_read_wait);
static int wait_flag = 0;
 
ssize_t ywwh_vpu_pow_read(struct file *filp, char __user *buf,
                      size_t count, loff_t *ppos)
{
    wait_event_interruptible(vpu_pow_read_wait, wait_flag);
    wait_flag = 0;
    if (copy_to_user(buf, EVENT_POW_OFF, strlen(EVENT_POW_OFF)))
        return -EFAULT;
 
    return strlen(EVENT_POW_OFF);
}
 
static struct file_operations ywwh_vpu_pow_ops = {
    .owner = THIS_MODULE,
    .read = ywwh_vpu_pow_read,
};
 
static struct miscdevice miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEV_NAME,
    .fops = &ywwh_vpu_pow_ops,
    .nodename = DEV_NAME,
};
 
static irqreturn_t vpu_pow_off_irq(int irq, void *dev_id)
{
    printk("interrupt happend!\n");
    wake_up(&vpu_pow_read_wait);
    wait_flag = 1;
 
    return IRQ_HANDLED;
}
 
static int vpu_pow_init(void)
{
    printk("irq = %u\n", VPU_POW_INT);
    mxc_iomux_v3_setup_pad(MX6DL_PAD_GPIO_16__GPIO_7_11);
    gpio_request(VPU_POW_INT_PIN, "vpu_pow_int");
    gpio_direction_input(VPU_POW_INT_PIN);
    printk("Get gpio val = %u\n", gpio_get_value_cansleep(VPU_POW_INT_PIN));
    gpio_free(VPU_POW_INT_PIN);
 
    int ret = request_irq(VPU_POW_INT, vpu_pow_off_irq,
        IRQF_TRIGGER_FALLING, "vpu_pow_off_irq", NULL);
    if (ret)
        return ret;
 
    ret = misc_register(&miscdev);
    if (ret)
        return ret;
 
    return 0;
}
 
static void vpu_pow_exit(void)
{
    misc_deregister(&miscdev);
}
 
module_init(vpu_pow_init);
module_exit(vpu_pow_exit);
/*
--------------------- 
作者：王凯_光谷 
来源：CSDN 
原文：https://blog.csdn.net/kao2406/article/details/51222570 
版权声明：本文为博主原创文章，转载请附上博文链接！
*/