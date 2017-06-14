#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

static const char dir_name[] = "zuehlke";
static const char file1_name[] = "id";
static const char file2_name[] = "jiffies";
static const char file3_name[] = "foo";
static struct dentry *debugFS;
static struct dentry *id;
static struct dentry *foo;
static void *data;
static const char buffer[] = "ZuehlkeCamp2017\n\0";
static const int length_bf = strlen(buffer);

/* File zuehlke */
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

/* File jiffies*/
static ssize_t jiffies_read(struct file *fd,
			 char __user *buf,
			 size_t len,
			 loff_t *ppos)
{
	u64 j = get_jiffies_64();
	char jbuf[256];
	sprintf(jbuf, "%llu", j); // or atoi?

	return simple_read_from_buffer(buf, len, ppos,
				       jbuf, strlen(jbuf));
}
static const struct file_operations jiffies_fops = {
	.read = jiffies_read,
};

/* File foo*/
static ssize_t foo_read(struct file *fd,
			 char __user *buf,
			 size_t len,
			 loff_t *ppos)
{
	return simple_read_from_buffer(buf, len, ppos,
				       buffer, strlen(buffer));
}
static const struct file_operations foo_fops = {
	.read = foo_read,
};

//---------------------------------------------------------------------------
static int __init debugFS_init(void)
{
	/* Create directory */
	debugFS = debugfs_create_dir(dir_name, NULL);
	if (IS_ERR_OR_NULL(debugFS))
		return -ENODEV;

	/* Create file "id" */
	id = debugfs_create_file(file1_name, 0666,
							debugFS, data, &zuehlke_fops);
	if (IS_ERR_OR_NULL(id))
		goto error_handler;

	/* Create file "jiffies" */
	jiffies = debugfs_create_file(file2_name, 0444,
							debugFS, data, &jiffies_fops);
	if (IS_ERR_OR_NULL(jiffies))
		goto error_handler; //bla bla

	/* Create file "foo" */
	foo = debugfs_create_file(file3_name, 0644,
							debugFS, data, &foo_fops);
	if (IS_ERR_OR_NULL(foo))
		goto error_handler; //bla bla



	return 0;

//debugfs_remove(foo);
//debugfs_remove(jiffies);
//debugfs_remove(id);
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
