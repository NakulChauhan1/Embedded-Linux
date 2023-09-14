#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>

static dev_t dev = 0;
static unsigned int count = 3;
static struct class *cls = NULL;
static struct device *d = NULL;
static struct cdev char_device;

static int driver_open (struct inode *in, struct file *f)
{
    printk("Hello World: Device file opened \n");
    return 0;
}

static ssize_t driver_read (struct file *f, char __user *data, size_t size, loff_t *off)
{
    printk("Hello World: Device file read \n");
    return 0;
}

static ssize_t driver_write (struct file *, const char __user *data, size_t size, loff_t *off)
{
    printk("Hello World: Device file written with %s(%ld)\n", data, size);
    return size;
}

static int driver_release (struct inode *, struct file *)
{
    printk("Hello World: Device file closed \n");
    return 0;
}

/*
static int driver_ioctl (struct file *, unsigned int, unsigned long)
{
    printk("Hello World: Device file sent small data (ioctl) \n");
}
*/

static const struct file_operations fops = 
{                              \
    .owner      = THIS_MODULE,                                         \
    .open       = &driver_open,                                        \
    .read       = &driver_read,                                        \
    .write      = &driver_write,                                       \
    .release    = &driver_release,                                       \
};


static int __init hello_world_init(void)
{
    unsigned int baseminor = 0;
    int r;

    //register a range of char device numbers (Generate major number, minor number)
    if(alloc_chrdev_region(&dev, baseminor, count, "helloworld") < 0)
    {
        printk("Hello World Char device number registration failed \n");
        return -1;
    }

    printk("Hello World Kernel Module Inserted succesffully \n");
    printk("Major = %d, Minor = %d \n",MAJOR(dev), MINOR(dev));

    /*
    Check the major number using cat /proc/devices, note: device file is not created yet, so nothing will be under /dev

    
    Manual way of creating device file: mknod -m <permissions> <name> <device type> <major> <minor>
    Automatic creation of device files can be handled with udev. Udev is the device manager for the Linux kernel that creates/removes device nodes in the /dev directory dynamically.
    */
   
   /*Creating struct class*/
   cls = class_create(THIS_MODULE, "Others");
        
    if(IS_ERR(cls))
    {
        printk("Cannot create the struct class for device\n");
        goto r_class;
    }

    /*Creating device (device file)*/    
    d = device_create(cls, NULL, dev, NULL, "helloworld");

    if(IS_ERR(d))
    {
        printk("Cannot create the Device\n");
        goto r_device;
    }

    // Initialize cdev structure
    cdev_init(&char_device, &fops);

    // add (tell about) fop initialized cdev structure to kernel 
    r = cdev_add(&char_device, dev, count);

    if(r<0)
    {
        printk("cdev addition to the kernal failed \n");
        goto r_cdev;
    }

    return 0;

    r_cdev:
        device_destroy(cls, dev);

    r_device:
        class_destroy(cls);
    
    r_class:
        unregister_chrdev_region(dev,1);

    return -1;
}

static void __exit hello_world_exit(void)
{
    device_destroy(cls, dev);
    class_destroy(cls);
    unregister_chrdev_region(dev, count);
    cdev_del(&char_device);

    printk("Device Destroyed,  Class Destroyed, Dev unregistered, cdev removed from system \n");
    printk("By By World \n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_AUTHOR("Nakul Chauhan");
MODULE_LICENSE("GPL");