#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

static struct miscdevice misc_dev;
char *buffer = "ZuehlkeCamp2017\n";

//static ssize_t simple_write(void *to, size_t available, loff_t *ppos,
//                         const void __user *from, size_t count)
//{
//	return simple_write_to_buffer(to, available, ppos, from, count);
//}
static ssize_t simple_write(struct file *file, const char *from, size_t count,
												loff_t *ppos)
{
	return simple_write_to_buffer(buffer, strlen(buffer), ppos, from, count);
}
//static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

//static ssize_t simple_read(void __user *to, size_t count, loff_t *ppos,
//                        const void *from, size_t available)
//{
//	return simple_read_from_buffer(to, count, ppos, from, available);
//}


static ssize_t simple_read(struct file *file, char __user *to, size_t count,
											loff_t *ppos)
{
	return simple_read_from_buffer(to, count, ppos, buffer, strlen(buffer));
}
//static ssize_t device_read(struct file *, char *, size_t, loff_t *);

static const struct file_operations zuehlke_fops = {
	.read = simple_read,
	.write = simple_write,
};

static int __init misc_dev_init(void)
{
	int ret;

	misc_dev.minor = MISC_DYNAMIC_MINOR;
	misc_dev.name = "zuehlke";
	misc_dev.fops = &zuehlke_fops;
	misc_dev.mode = 0666;

	ret = misc_register(&misc_dev);
	if (ret) {
		pr_info("Misc driver can not be registered\n");
		return ret;
	}
	pr_info("Misc driver is registered.\n");
	return 0;
}
static void __exit misc_dev_exit(void)
{
	misc_deregister(&misc_dev);
	pr_info("Misc driver is deregistered.\n");
}
module_init(misc_dev_init);
module_exit(misc_dev_exit);
