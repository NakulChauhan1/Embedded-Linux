
#include<linux/module.h>
#include<linux/kernel.h>

static int __init hello_world_init(void)
{
    printk("Hello World \n");
    return 0;
}

static void __exit hello_world_exit(void)
{
    printk("By By World \n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_AUTHOR("Nakul Chauhan");
MODULE_LICENSE("GPL");