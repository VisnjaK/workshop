#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

static const char dir_name[] = "zuehlke";
static struct dentry *debugFS;

static int __init debugFS_init(void)
{
	debugFS = debugfs_create_dir(dir_name, NULL);
	if (IS_ERR_OR_NULL(debugFS))
		return -ENODEV;

	return 0;
}
static void __exit debugFS_exit(void)
{
	debugfs_remove_recursive(debugFS);
}

module_init(debugFS_init);
module_exit(debugFS_exit);
