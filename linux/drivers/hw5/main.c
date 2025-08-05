#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/fs.h>
#include <linux/device.h>

#define HW5_BUF_SIZE 32
#define MAJOR_NUMBER 177
#define MINOR_NUMBER 34

static DEFINE_RWLOCK(hw5_device_rwlock); // state spinlock
static char hw5_device_buf[HW5_BUF_SIZE] = {0, };


/*
 * __user *buf : user buffer
 * hw5_device_buf : kernel buffer
 *
 */

static ssize_t hw5_device_read(struct file *fp, char __user *buf, size_t len, loff_t *ppos) {
	int written_bytes = 0;

	read_lock(&hw5_device_rwlock);	
	
	if(HW5_BUF_SIZE <= len + *ppos) {
		len = HW5_BUF_SIZE - *ppos;
	}
	
	// copy_to_user : returns bytes number that read failed
	written_bytes = len - copy_to_user(buf, hw5_device_buf + *ppos, len);
	*ppos += written_bytes;

	read_unlock(&hw5_device_rwlock);

	return written_bytes;
}

static ssize_t hw5_device_write(struct file *fp, const char __user *buf, size_t len, loff_t *ppos) {
	int read_bytes = 0;

	write_lock(&hw5_device_rwlock);
	
	if (HW5_BUF_SIZE <= len + *ppos) {
		len = HW5_BUF_SIZE - *ppos;
	}

	read_bytes = len - copy_from_user(hw5_device_buf + *ppos, buf, len);
	*ppos += read_bytes;

	write_unlock(&hw5_device_rwlock);

	return read_bytes;
}


static struct file_operations hw5_fops = {
	.read = hw5_device_read,
	.write = hw5_device_write,
};


static int __init hw5_driver_init(void) {
	printk(KERN_DEBUG "%s\n", __func__);

	return register_chrdev(MAJOR_NUMBER, "hw5 device", &hw5_fops);
}

static void __exit hw5_driver_exit(void) {
	printk(KERN_DEBUG "%s\n", __func__);

	unregister_chrdev(MAJOR_NUMBER, "hw5 device");
}

module_init(hw5_driver_init);
module_exit(hw5_driver_exit);

MODULE_AUTHOR("Jin Minu");
MODULE_DESCRIPTION("home work5 module");
MODULE_LICENSE("GPL v2");
