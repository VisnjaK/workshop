#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

static struct miscdevice misc_dev;
static const char buffer[] = "ZuehlkeCamp2017\n\0";
static const int length_bf = strlen(buffer);


#if 0
static ssize_t simple_write(struct file *file, const char *from, size_t count,
	loff_t *ppos)
{
	return simple_write_to_buffer(buffer, strlen(buffer), ppos, from, count);
}

static ssize_t simple_read(struct file *file, char __user *to, size_t count,
											loff_t *ppos)
{
	return simple_read_from_buffer(to, count, ppos, buffer, strlen(buffer));
}
#endif
static ssize_t simple_read(struct file *fd,
			 char __user *buf,
			 size_t len,
			 loff_t *ppos)
{
	return simple_read_from_buffer(buf, len, ppos,
				       buffer, strlen(buffer));
}

static ssize_t simple_write(struct file *fd,
			  const char __user *buf,
			  size_t len,
			  loff_t *ppos)
{
	char temp_buf[length_bf + 1];
	int len_write = simple_write_to_buffer(temp_buf,
					       length_bf,
					       ppos, buf, len);
	if (len_write < 0)
		return len_write;
	if ((len_write != len) || (len_write != length_bf))
		return -EINVAL;
	if (strncmp(temp_buf, buffer, length_bf) == 0)
		return length_bf;
	else
		return -EINVAL;

}


static const struct file_operations zuehlke_fops = {
	.read = simple_read,
	.write = simple_write,
};


#if 1
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
#endif

#if 0
static struct miscdevice misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "zuehlke",
	.fops = &zuehlke_fops,
	.mode = 0666
};
#endif


module_init(misc_dev_init);
module_exit(misc_dev_exit);
