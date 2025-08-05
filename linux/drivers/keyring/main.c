#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/hashtable.h>


#define DEVICE_NAME "keyring"
#define CTL_DEVICE_NAME "keyringctl"
#define CLASS_NAME "key"

#define IOCTL_MAGIC 'C'
#define IOCTL_ADD _IOW(IOCTL_MAGIC, 0, int)
#define IOCTL_DEL _IOW(IOCTL_MAGIC, 1, int)

#define BUF_SIZE 32

static int device_major;
static struct class *key_class;
static struct device *key_device;

struct device_data {
	int key;
	char passwd[BUF_SIZE];
	struct device *device;
	struct hlist_node node;
};

static DEFINE_HASHTABLE(device_table, 8);
static DEFINE_RWLOCK(device_rwlock);

static struct device_data *get_device_data(unsigned long key) {
	struct device_data *data;
	hash_for_each_possible(device_table, data, node, key) {
		if (data->key != key) {
			continue;
		}

		return data;
	}

	return NULL;
}

static ssize_t device_read(struct file *fp, char __user *buf, size_t len, loff_t *ppos) {
	int minor = iminor(fp->f_inode);
	int written_bytes = 0;
	struct device_data *data;
	read_lock(&device_rwlock);

	
	data = get_device_data(minor);
	if (data == NULL) {
		read_unlock(&device_rwlock);
		return -1;
	}

	if (BUF_SIZE <= len + *ppos) {
		len = BUF_SIZE - *ppos;
	}

	written_bytes = len - copy_to_user(buf, data->passwd + *ppos, len);
	*ppos += written_bytes;
	read_unlock(&device_rwlock);

	return written_bytes;

}

static ssize_t device_write(struct file *fp, const char __user *buf, size_t len, loff_t *ppos) {
	int minor = iminor(fp->f_inode);
	struct device_data *data;
	int read_bytes = 0;

	write_lock(&device_rwlock);

	data = get_device_data(minor);
	if (data == NULL) {
		write_unlock(&device_rwlock);
		return -1;
	}

	if (BUF_SIZE <= len + *ppos) {
		len = BUF_SIZE - *ppos;
	}

	read_bytes = len - copy_from_user(data->passwd, buf + *ppos, len);
	*ppos += read_bytes;

	write_unlock(&device_rwlock);
	
	return read_bytes;
}

static long device_ioctl(struct file *fp, unsigned int cmd, unsigned long arg) {

	int ret = 0;
	int bkt;
	struct device_data *data;
	struct hlist_node *tmp;

	switch (cmd) {
		case IOCTL_ADD:
			if (arg >= 1048576) {
				ret = -EINVAL;
				break;
			}

			write_lock(&device_rwlock);
			data = get_device_data(arg + 1);

			if (data != NULL) {
				ret = -EEXIST;
				write_unlock(&device_rwlock);
				break;
			}

			data = kmalloc(sizeof(struct device_data), GFP_KERNEL);
			data->device = device_create(key_class, NULL, MKDEV(device_major, arg + 1), NULL, "%s%ld", DEVICE_NAME, arg);

			if (IS_ERR(data->device)) {
				ret = PTR_ERR(data->device);
				kfree(data);
				write_unlock(&device_rwlock);
				break;
			}
	
			data->key = arg + 1;
			memset(data->passwd, 0, BUF_SIZE);
			hash_add(device_table, &data->node, data->key);
			write_unlock(&device_rwlock);
			break;

		case IOCTL_DEL:
			ret = -ENOENT;
			write_lock(&device_rwlock);
			hash_for_each_safe(device_table, bkt, tmp, data, node) {
				if (data->key == arg + 1) {
					hash_del(&data->node);
					device_destroy(key_class, MKDEV(device_major, data->key));
					kfree(data);
					
					ret = 0;
				}
			}
			write_unlock(&device_rwlock);
			break;
		default:
			ret = -EINVAL;
	}
	return ret;
}


static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = device_ioctl,
};

static int __init keyring_module_init(void) {
	int ret = 0;
	device_major = register_chrdev(0, CTL_DEVICE_NAME, &fops);

	if (device_major < 0) {
		printk(KERN_ERR "%s : Failed to get major number", DEVICE_NAME);
		ret = device_major;
		goto err_register_chrdev;
	}

	key_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(key_device)) {
		ret = PTR_ERR(key_device);
		goto err_device;
	}

	return ret;

err_device:
	class_destroy(key_class);
err_class:
	unregister_chrdev(device_major, DEVICE_NAME);
err_register_chrdev:
	return ret;
}

static void __exit keyring_module_exit(void) {
	int bkt;
	struct device_data *data;
	struct hlist_node *tmp;

	printk(KERN_DEBUG "%s", __func__);

	hash_for_each_safe(device_table, bkt, tmp, data, node) {
		hash_del(&data->node);
		device_destroy(key_class, MKDEV(device_major, data->key));
		kfree(data);
	}

	device_destroy(key_class, MKDEV(device_major, 0));
	class_destroy(key_class);
	unregister_chrdev(device_major, DEVICE_NAME);
}

module_init(keyring_module_init);
module_exit(keyring_module_exit);

MODULE_AUTHOR("Jin Minu");
MODULE_DESCRIPTION("HW5 : keyring, keyringctl");
MODULE_LICENSE( "GPL v2");
