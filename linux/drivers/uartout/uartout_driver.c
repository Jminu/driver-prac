#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/hashtable.h>
#include <linux/spinlock.h>
#include <asm/io.h>

#define DEVICE_NAME "uartout_device"
#define CLASS_NAME "uartout_class"

static int device_major;
static DEFINE_RWLOCK(device_rwlock);
static struct device *uartout_device;
static struct class *uartout_class;

static ssize_t uartout_device_write(struct file *fp, const char __user *user_buf, size_t len, loff_t *ppos)
{
	int read_bytes = 0;
	char *kernel_buf;
	char *log;
	char *ptr;
	char *addr;

	write_lock(&device_rwlock);
	read_bytes = len - copy_from_user(kernel_buf, user_buf, len);
	kernel_buf[read_bytes] = '\0';

	log = kmalloc(read_bytes, GFP_KERNEL);
	snprintf(log, read_bytes, "%s", kernel_buf);
	kfree(kernel_buf);
	addr = ioremap(0x09000000, 0x1000);
	for (ptr = log; ptr != NULL; ptr++)
	{
		*addr = *ptr;
	}
	iounmap(addr);
	kfree(log);
	write_unlock(&device_rwlock);

	return read_bytes;
}

static struct file_operations fops = {
	.write = uartout_device_write,
};

static int __init uartout_module_init(void)
{
	/*
	 *  No Err processing
	 */
	
	int ret = 0;

	device_major = register_chrdev(0, DEVICE_NAME, &fops);
	uartout_class = class_create(THIS_MODULE, CLASS_NAME);
	uartout_device = device_create(uartout_class, NULL, MKDEV(device_major, 0), NULL, DEVICE_NAME);

	return ret;
}

static void __exit uartout_module_exit(void)
{
	device_destroy(uartout_class, MKDEV(device_major, 0));
	class_destroy(uartout_class);
	unregister_chrdev(device_major, DEVICE_NAME);
}

module_init(uartout_module_init);
module_exit(uartout_module_exit);

MODULE_AUTHOR("JIN MINU");
MODULE_DESCRIPTION("uart out driver");
MODULE_LICENSE("GPL v2");
