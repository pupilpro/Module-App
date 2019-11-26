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

/***************************************************************
文件名	: ledDrv.c
作者	  	: Mr.chen
版本	   	: V1.0
描述	   	: 字符驱动模板文件
其他	   	: 无
博客 	   	: https://me.csdn.net/u014133923
GitHub  : https://github.com/pupilpro
日志	   	: [create][2019.11.6]
***************************************************************/
#define ledDev_CNT					1		  	/* 设备号个数 */
#define ledDev_NAME			"ledDev"	/* 名字 */
 
/* 寄存器物理地址 */
#define APER_CLK_CTRL 		(0xF800012C)
#define OEN_2 					(0xE000A288)
#define DIRM_2 					(0xE000A284)
#define MASK_DATA_2_LSW (0xE000A010)

/* 映射后的寄存器虚拟地址指针 */
static void __iomem *pslcr;
static void __iomem *pOEN_2;
static void __iomem *pDIRM_2;
static void __iomem *pMASK_DATA_2_LSW;

/* ledDev设备结构体 */
struct ledDev_dev{
	dev_t devid;			/* 设备号 	 */
	struct cdev cdev;		/* cdev 	*/
	struct class *class;		/* 类 		*/
	struct device *device;	/* 设备 	 */
	int major;				/* 主设备号	  */
	int minor;				/* 次设备号   */
};

struct ledDev_dev ledDev;	/* led设备 */

/*
 * @description		: 打开设备
 * @param - inode 	: 传递给驱动的inode
 * @param - filp 	: 设备文件，file结构体有个叫做private_data的成员变量
 * 					  一般在open的时候将private_data指向设备结构体。
 * @return 			: 0 成功;其他 失败
 */
static int mould_open(struct inode *inode, struct file *filp)
{
	int val;
	filp->private_data = &ledDev; /* 设置私有数据 */
	/*设置初始状态off*/
	val = readl(pslcr);
	val &= ~(1 << 22); /* 清除以前的设置 */
	val |= (1 << 22); /* 设置新值 */
	writel(val, pslcr);
	
	val = readl(pOEN_2);
	val |= (0x0f); /* 设置新值 */
	writel(val, pOEN_2);
	
	val = readl(pDIRM_2);
	val |= (0x0f); /* 设置新值 */
	writel(val, pDIRM_2);
	
	val = readl(pMASK_DATA_2_LSW);
	val &= ~(0x0f); /* 清除以前的设置 */
	val |= (0x0f); /* 设置新值 */
	writel(val, pMASK_DATA_2_LSW);
	
	printk("mould_open......yes\r\n");
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
static ssize_t mould_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
	printk("mould_read......yes\r\n");
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
static ssize_t mould_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
	int retvalue;
	u32 val;
	unsigned char data;

	retvalue = copy_from_user(&data, buf, cnt);
	if(retvalue < 0) {
		printk("kernel write failed!\r\n");
		return -EFAULT;
	}
	switch(data)
	{
		case 0:
				val = readl(pMASK_DATA_2_LSW);
				val &= ~(0x0f); 
				val |= (0x0e); 
				writel(val, pMASK_DATA_2_LSW);
			break;
		case 1:
				val = readl(pMASK_DATA_2_LSW);
				val &= ~(0x0f); 
				val |= (0x0d);
				writel(val, pMASK_DATA_2_LSW);
			break;
		case 2:
				val = readl(pMASK_DATA_2_LSW);
				val &= ~(0x0f); 
				val |= (0x0b); 
				writel(val, pMASK_DATA_2_LSW);
			break;
		case 3:
				val = readl(pMASK_DATA_2_LSW);
				val &= ~(0x0f);
				val |= (0x07); 
				writel(val, pMASK_DATA_2_LSW);
			break;
		default:
			break;
	}
	printk("mould_write......yes\r\n");
	return 0;
}

/*
 * @description		: 关闭/释放设备
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int mould_release(struct inode *inode, struct file *filp)
{
	printk("mould_release......yes\r\n");
	return 0;
}

/* 设备操作函数 */
static struct file_operations ledDev_fops = {
	.owner = THIS_MODULE,
	.open = mould_open,
	.read = mould_read,
	.write = mould_write,
	.release = mould_release,
};

/*
 * @description	: 驱动入口函数
 * @param 		: 无
 * @return 		: 无
 */
static int __init mould_init(void)
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
	
	/* 寄存器地址映射 */
  	//newAddr = ioremap(oldAddr, 4);
	pslcr = ioremap(APER_CLK_CTRL, 4);
	pOEN_2 = ioremap(OEN_2, 4);
	pDIRM_2 = ioremap(DIRM_2 , 4);
	pMASK_DATA_2_LSW = ioremap(MASK_DATA_2_LSW , 4);
	
	printk("mould_init......yes\r\n");	
	return 0;
}

/*
 * @description	: 驱动出口函数
 * @param 		: 无
 * @return 		: 无
 */
static void __exit mould_exit(void)
{
	/* 取消映射 */
	//iounmap(newAddr);
	iounmap(pslcr);
	iounmap(pOEN_2);
	iounmap(pDIRM_2);
	iounmap(pMASK_DATA_2_LSW);

	/* 注销字符设备驱动 */
	cdev_del(&ledDev.cdev);/*  删除cdev */
	unregister_chrdev_region(ledDev.devid, ledDev_CNT); /* 注销设备号 */

	device_destroy(ledDev.class, ledDev.devid);
	class_destroy(ledDev.class);
	
	printk("mould_exit......yes\r\n");
}

module_init(mould_init);
module_exit(mould_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mr.chen");
