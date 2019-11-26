#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>

/***************************************************************
文件名	: ledDrv.c
作者	  	: Mr.chen
版本	   	: V1.0
描述	   	: led驱动文件
其他	   	: 无
博客 	   	: https://me.csdn.net/u014133923
GitHub  : https://github.com/pupilpro
日志	   	: [create][2019.11.25]
***************************************************************/
#define ledDev_CNT					1		  	/* 设备号个数 */
#define ledDev_NAME			"ledDev"	/* 名字 */

/* ledDev设备结构体 */
struct ledDev_dev{
	dev_t devid;			/* 设备号 	 */
	struct cdev cdev;		/* cdev 	*/
	struct class *class;		/* 类 		*/
	struct device *device;	/* 设备 	 */
	int major;				/* 主设备号	  */
	int minor;				/* 次设备号   */
	struct device_node *nd; /*  设备节点 */
	int led_gpio; /* led  所使用的 GPIO  编号 */
};

struct ledDev_dev ledDev;	/* led设备 */

/*
 * @description			: 打开设备
 * @param - inode 	: 传递给驱动的inode
 * @param - filp 		: 设备文件，file结构体有个叫做private_data的成员变量
 * 					  			一般在open的时候将private_data指向设备结构体。
 * @return 				: 0 成功;其他 失败
 */
static int led_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &ledDev; /* 设置私有数据 */
	
	printk("led_open......yes\r\n");
	return 0;
}

/*
 * @description		: 从设备读取数据 
 * @param - filp 	: 要打开的设备文件(文件描述符)
 * @param - buf 	: 返回给用户空间的数据缓冲区
 * @param - cnt 	: 要读取的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 读取的字节数，如果为负值，表示读取失败
 */
static ssize_t led_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
	printk("led_read......yes\r\n");
	return 0;
}

/*
 * @description		: 向设备写数据 
 * @param - filp 	: 设备文件，表示打开的文件描述符
 * @param - buf 	: 要写给设备写入的数据
 * @param - cnt 	: 要写入的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 写入的字节数，如果为负值，表示写入失败
 */
static ssize_t led_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
	int retvalue;
	unsigned char data;
	struct ledDev_dev *dev = filp->private_data;
	retvalue = copy_from_user(&data, buf, cnt);
	if(retvalue < 0) {
		printk("kernel write failed!\r\n");
		return -EFAULT;
	}
	switch(data)
	{
		case 0:
				gpio_set_value(ledDev.led_gpio, 0); /*  打开 LED  灯 */
			break;
		case 1:
				gpio_set_value(ledDev.led_gpio, 1); /*  关闭 LED  灯 */
			break;
		case 2:
				gpio_set_value(ledDev.led_gpio, 0); /*  打开 LED  灯 */
			break;
		case 3:
				gpio_set_value(ledDev.led_gpio, 1); /*  关闭 LED  灯 */
			break;
		default:
			break;
	}
	printk("led_write......yes\r\n");
	return 0;
}

/*
 * @description		: 关闭/释放设备
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int led_release(struct inode *inode, struct file *filp)
{
	printk("led_release......yes\r\n");
	return 0;
}

/* 设备操作函数 */
static struct file_operations ledDev_fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.read = led_read,
	.write = led_write,
	.release = led_release,
};

/*
 * @description	: 设备树信息相关处理
 * @param 		: 无
 * @return 			: 0 成功;其他 失败
 */
 static int opt_devicetree(void)
 {
	int ret;
	
	/*  获取设备树中的属性数据 */
	/* 1 、获取设备节点：zynq_led */
	ledDev.nd = of_find_node_by_path("/zynq_led1");
	if(ledDev.nd == NULL) {
		printk("zynq_led node not find!\r\n");
		return -EINVAL;
	 } else {
		printk("zynq_led node find!\r\n");
	 }
	 
	 /* 2、 获取设备树中的 gpio 属性，得到 LED 所使用的 LED 编号 */
	 ledDev.led_gpio = of_get_named_gpio(ledDev.nd, "led-gpio", 0);
	 if(ledDev.led_gpio < 0) {
		printk("can't get led-gpio\r\n");
		return -1;
	 }
	 printk("led-gpio num = %d\r\n", ledDev.led_gpio);
	 
	 /* 3、申请 GPIO 资源 */
	ret = gpio_request(ledDev.led_gpio, "run");
	 if(ret != 0){
		printk("can't request gpio!\r\n");
	 }
	 
	 /* 4、设置 GPIO 为输出 */
	 ret = gpio_direction_output(ledDev.led_gpio, 1);
	 if(ret < 0) {
		printk("can't set gpio!\r\n");
	}
	
	 /* 5、设置 GPIO 初始状态 */
	gpio_set_value(ledDev.led_gpio, 1); /*  默认关闭 LED 灯 */

	return 0;
 }

/*
 * @description	: 驱动入口函数
 * @param 		: 无
 * @return 			: 无
 */
static int __init led_init(void)
{
	/* 注册字符设备驱动 */
	/* 1、创建设备号 */
	if (ledDev.major) {		/*  定义了设备号 */
		ledDev.devid = MKDEV(ledDev.major, 0);
		register_chrdev_region(ledDev.devid, ledDev_CNT, ledDev_NAME);
	} else {						/* 没有定义设备号 */
		alloc_chrdev_region(&ledDev.devid, 0, ledDev_CNT, ledDev_NAME);	/* 申请设备号 */
		ledDev.major = MAJOR(ledDev.devid);	/* 获取分配号的主设备号 */
		ledDev.minor = MINOR(ledDev.devid);	/* 获取分配号的次设备号 */
	}
	
	/* 2、初始化cdev */
	ledDev.cdev.owner = THIS_MODULE;
	cdev_init(&ledDev.cdev, &ledDev_fops);
	
	/* 3、添加一个cdev */
	cdev_add(&ledDev.cdev, ledDev.devid, ledDev_CNT);

	/* 自动生成设备节点 */
	/* 1、创建类 */
	ledDev.class = class_create(THIS_MODULE, ledDev_NAME);
	if (IS_ERR(ledDev.class)) {
		return PTR_ERR(ledDev.class);
	}

	/* 2、创建设备 */
	ledDev.device = device_create(ledDev.class, NULL, ledDev.devid, NULL, ledDev_NAME);
	if (IS_ERR(ledDev.device)) {
		return PTR_ERR(ledDev.device);
	}
	
	/* 设备树信息处理 */
	opt_devicetree();
	
	printk("led_init......yes\r\n");	
	return 0;
}

/*
 * @description	: 驱动出口函数
 * @param 		: 无
 * @return 			: 无
 */
static void __exit led_exit(void)
{
	gpio_free(ledDev.led_gpio);
	/* 注销字符设备驱动 */
	cdev_del(&ledDev.cdev);/*  删除cdev */
	unregister_chrdev_region(ledDev.devid, ledDev_CNT); /* 注销设备号 */

	device_destroy(ledDev.class, ledDev.devid);
	class_destroy(ledDev.class);
	
	printk("led_exit......yes\r\n");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mr.chen");

/* END OF FILE ---------------------------------------------------------------*/