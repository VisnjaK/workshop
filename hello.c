#include <linux/init.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/types.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

static u64 start_ms;
static u64 end_ms;

static int __init hello_init(void)
{
	start_ms = jiffies_to_msecs(get_jiffies_64());
	pr_info("Hello world!\n");
	return 0;
}
static void __exit hello_exit(void)
{
	end_ms = jiffies_to_msecs(get_jiffies_64());
	pr_info("Time passed [s]: %llu\n", (end_ms - start_ms) / HZ);
	pr_info("See ya world!\n");
}
module_init(hello_init);
module_exit(hello_exit);
