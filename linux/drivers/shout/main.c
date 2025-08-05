#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/hashtable.h>

#define DEVICE_NAME "shout"
#define CLASS_NAME "shout_class"

static struct device *device;
static struct class *class;
static int device_major;
static DEFINE_RWLOCK(device_rwlock);


static inline void smh_putc(unsigned char c) {
	asm volatile("mov x1, %0\n mov x0, #3\n"
			"hlt 0xf000\n"
			:: "r" (&c): "x0", "x1", "memory");
}

static ssize_t device_write(struct file *fp, const char __user *user_buf, size_t len, loff_t *ppos) {
	int write_bytes = 0;
	char *log;
	char *kernel_buf;
	char *ptr;

	write_lock(&device_rwlock);
	kernel_buf = kmalloc(len + 1, GFP_KERNEL);
	write_bytes = len - copy_from_user(kernel_buf, user_buf, len);
	
	kernel_buf[write_bytes] = '\0';
	log = kmalloc(write_bytes + 1, GFP_KERNEL);
	
	snprintf(log, write_bytes, );
	kfree(kernel_buf);

	for (ptr = log; *ptr != NULL; ptr++) {
		smh_putc(*ptr);
	}
	kfree(log);

	write_unlock(&device_rwlock);

	return write_bytes;
}

static struct file_operations fops = {
	.write = device_write,
};

static int __init module_init(void) {
	int ret = 0;
	
	device_major = register_chrdev(0, DEVICE_NAME, &fops);
	if (device_major < 0) {
		printk(KERN_ERR "%s : failed to assign major number!\n", DEVICE_NAME);
		ret = device_major;
		goto err_register_chrdev;
	}

	class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(class)) {
		printk(KERN_ERR "%s: failed to create class", DEVICE_NAME);
		ret = PTR(class);
		goto err_class;
	}

	device = device_create(class, NULL, MKDEV(device_major, 0), NULL, DEVICE_NAME);
	if (IS_ERR(device)) {
		ret = PTR_ERR(device);
		goto err_device;
	}

	return ret;

err_device:
	class_destroy(class);
err_class:
	unregister_chrdev(device_major, DEVICE_NAME);
err_register_chrdev:
	return ret;

}

static void __exit_module_exit(void) {
	device_destroy(class, MKDEV(device_major, 0));
	class_destroy(class);
	unregister_chrdev(device_major, DEVICE_NAME);
}

module_init(module_init);
module_exit(module_exit);

MODULE_AUTHOR("Jin Minu");
MODULE_DESCRIPTION("QEMU semihosting shout module");
MODULE_LICENSE("GPL v2");
