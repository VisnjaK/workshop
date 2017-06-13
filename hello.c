#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

__init static int hello_init(void)
{
  pr_info("Hello world!\n");
  return 0;
}
__exit static void hello_exit(void)
{
  pr_info("See ya world!\n");
  return;
}
module_init(hello_init);
module_exit(hello_exit);
