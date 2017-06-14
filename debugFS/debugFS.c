#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/rwlock.h>
#include <linux/slab.h>
#include <linux/capability.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

static const char dir_name[] = "zuehlke";
static const char file1_name[] = "id";
static const char file2_name[] = "jiffies";
static const char file3_name[] = "foo";
static struct dentry *debugFS; //dentry could be reused
static struct dentry *id;
static struct dentry *j;
static struct dentry *foo;
static const char buffer[] = "ZuehlkeCamp2017\n\0";
static const int length_bf = strlen(buffer);
static rwlock_t data_lock;
static uint8_t *data;

//in init:

//foo create file
//...
//foo read:
//read_lock(&data_lock)
//res = simple_read_from_buffer(buf, len, off, data, PAGE_SIZE )..
//read_unlock(&data_lock)
//write:
//if (!capable(CAP_SYS_ADMIN))
//return -EPERM
//write_lock(&data_lock)
//res = simple_write_to_buffer(daya, PAGE_SIZE,..)
//write_unlock

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
	sprintf(jbuf, "%llu\n", j); // or atoi?

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
	int res = 0;
	read_lock(&data_lock);
	res = simple_read_from_buffer(buf, len, ppos, data, PAGE_SIZE);
	read_unlock(&data_lock);
	return res;
}
static ssize_t foo_write(struct file *fd,
			  const char __user *buf,
			  size_t len,
			  loff_t *ppos)
{
	int res = 0;
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	write_lock(&data_lock);
	res = simple_write_to_buffer(data, PAGE_SIZE, ppos, buf, len);
	write_unlock(&data_lock);
	return res;
}
static const struct file_operations foo_fops = {
	.read = foo_read,
	.write = foo_write,
};

//---------------------------------------------------------------------------
static int __init debugFS_init(void)
{
	data = NULL;
	rwlock_init(&data_lock);
	data = kzalloc(PAGE_SIZE, GFP_KERNEL);

	/* Create directory */
	debugFS = debugfs_create_dir(dir_name, NULL);
	if (IS_ERR_OR_NULL(debugFS))
		return -ENODEV;

	/* Create file "id" */
	id = debugfs_create_file(file1_name, 0666,
							debugFS, NULL, &zuehlke_fops);
	if (IS_ERR_OR_NULL(id))
		goto error_handler;

	/* Create file "jiffies" */
	j = debugfs_create_file(file2_name, 0444,
							debugFS, NULL, &jiffies_fops);
	if (IS_ERR_OR_NULL(j))
		goto error_handler;

	/* Create file "foo" */
	foo = debugfs_create_file(file3_name, 0644,
							debugFS, data, &foo_fops);
	if (IS_ERR_OR_NULL(foo))
		goto error_handler;

	return 0;

error_handler:
	debugfs_remove_recursive(debugFS);
	if (data != NULL)
		kzfree(data);
	return -ENODEV;
}

static void __exit debugFS_exit(void)
{
	if (data != NULL)
		kzfree(data);
	debugfs_remove_recursive(debugFS);
}

module_init(debugFS_init);
module_exit(debugFS_exit);
