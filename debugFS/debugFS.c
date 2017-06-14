#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

static const char dir_name[] = "zuehlke";
static const char file_name[] = "id";
static struct dentry *debugFS;
static struct dentry *id;
static void *data;
static const char buffer[] = "ZuehlkeCamp2017\n\0";
static const int length_bf = strlen(buffer);

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

static int __init debugFS_init(void)
{
	debugFS = debugfs_create_dir(dir_name, NULL);
	if (IS_ERR_OR_NULL(debugFS))
		return -ENODEV;

	id = debugfs_create_file(file_name, 0444,
							debugFS, data, &zuehlke_fops);
	if (IS_ERR_OR_NULL(id))
		goto error_handler;


	return 0;

error_handler:
	debugfs_remove_recursive(debugFS);
	return -ENODEV;
}

static void __exit debugFS_exit(void)
{
	debugfs_remove_recursive(debugFS);
}

module_init(debugFS_init);
module_exit(debugFS_exit);
