#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/ioctl.h>

struct hw_ioctl_data
{
    int32_t value;
    char name[32];
};

#define HELLOWORLD_IOCTL_BASE   'H'
#define RD_VALUE                _IOR(HELLOWORLD_IOCTL_BASE, 0, struct hw_ioctl_data)
#define WR_VALUE                _IOW(HELLOWORLD_IOCTL_BASE, 1, struct hw_ioctl_data)

struct hw_ioctl_data value_ioctl = {
    .value = 111,
    .name = "Nakul Chauhan",
};

static dev_t dev_number = 0;
static unsigned int count = 3;
static struct class *cls = NULL;
static struct device *dev = NULL;
static struct cdev char_device;

ssize_t attr1_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t attr1_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t attr2_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t attr2_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t attr3_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t attr3_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);


static DEVICE_ATTR(name, 0644, attr1_show, attr1_store);
static DEVICE_ATTR(address, 0644, attr2_show, attr2_store);
static DEVICE_ATTR(number, 0644, attr3_show, attr3_store);

size_t kern_mem_size = 1024;
uint8_t *kern_data = NULL;

static int driver_open (struct inode *in, struct file *f)
{
    printk("Hello World: Device file opened \n");
    return 0;
}

static ssize_t driver_read (struct file *f, char __user *data, size_t size, loff_t *off)
{
    printk("Hello World: Device file read %s(%ld) \n", kern_data, size);
    return 0;
}

static ssize_t driver_write (struct file *, const char __user *data, size_t size, loff_t *off)
{
    printk("Hello World: Device file written with %s(%ld)\n", kern_data, size);
    return size;
}

static int driver_release (struct inode *, struct file *)
{
    printk("Hello World: Device file closed \n");
    return 0;
}

static long int driver_ioctl (struct file *, unsigned int command, unsigned long argument)
{
    switch (command)
    {
        case RD_VALUE:
            if(copy_to_user((int32_t*)argument, &value_ioctl, sizeof(value_ioctl)))
            {
                printk("ioctl data write: error \n");
                return 0;
            }
            break;
        
        case WR_VALUE:
            if(copy_from_user(&value_ioctl, (int32_t*)argument, sizeof(value_ioctl)))
            {
                printk("ioctl data read: error \n");
                return 0;
            }
            break;
        
        default:
            break;
    }
    printk("Hello World: Device file sent small data (ioctl) \n");
    return 0;
}

static const struct file_operations fops = 
{                              \
    .owner          = THIS_MODULE,
    .open           = &driver_open,
    .read           = &driver_read,
    .write          = &driver_write,
    .unlocked_ioctl = &driver_ioctl,
    .release        = &driver_release,
};


ssize_t attr1_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
    return 0;
}

ssize_t attr1_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
    return 0;
}

ssize_t attr2_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
    return 0;
}

ssize_t attr2_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
    return 0;
}

ssize_t attr3_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
    return 0;
}

ssize_t attr3_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
    return 0;
}


static int __init hello_world_init(void)
{
    unsigned int baseminor = 0;
    int r;

    //register a range of char device numbers (Generate major number, minor number)
    if(alloc_chrdev_region(&dev_number, baseminor, count, "helloworld") < 0)
    {
        printk("Hello World Char device number registration failed \n");
        return -1;
    }

    printk("Hello World Kernel Module Inserted succesffully \n");
    printk("Major = %d, Minor = %d \n",MAJOR(dev_number), MINOR(dev_number));

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

    /*Creating device (device file) using device number ie major number/ minor number 
      It gives pointer to device structure ie (struct device* ), and this is most important structure,
      which is bareminimum struct to represent a device. Refers to notes for understanding the power of struct device.
      note: how its created using device number ie maj,min number.  
      Todo: Write down usage of struct device.
    */
    dev = device_create(cls, NULL, dev_number, NULL, "helloworld");

    if(IS_ERR(dev))
    {
        printk("Cannot create the Device\n");
        goto r_device;
    }

    // Initialize cdev structure
    cdev_init(&char_device, &fops);

    // add (tell about) fop initialized cdev structure to kernel using device number
    r = cdev_add(&char_device, dev_number, count);

    if(r<0)
    {
        printk("cdev addition to the kernal failed \n");
        goto r_cdev;
    }

    kern_data = (uint8_t*)kmalloc (kern_mem_size, GFP_KERNEL);

    if (kern_data == NULL)
    {
        printk("Cannot allocate memory in kernel \n");
    }
    else
    {
        strcpy(kern_data, "Default data: Hello World");
        printk("kern_data: %s \n", kern_data);
    }

    //creating directories, attributes to get or set information to and fro from driver

    int ret;

    ret = device_create_file(dev, &dev_attr_name);
    if(ret)
    {
        printk("failed to create attribute_1 \n");
        device_remove_file(dev, &dev_attr_name);
        goto r_cdev; 
    }

    ret = device_create_file(dev, &dev_attr_address);
    if(ret)
    {
        printk("failed to create attribute_1 \n");
        device_remove_file(dev, &dev_attr_name);
        device_remove_file(dev, &dev_attr_address);
        goto r_cdev; 
    }

    ret = device_create_file(dev, &dev_attr_number);
    if(ret)
    {
        printk("failed to create attribute_1 \n");
        device_remove_file(dev, &dev_attr_name);
        device_remove_file(dev, &dev_attr_address);
        device_remove_file(dev, &dev_attr_number);
        goto r_cdev; 
    }

    return 0;

    r_cdev:
        device_destroy(cls, dev_number);

    r_device:
        class_destroy(cls);
    
    r_class:
        unregister_chrdev_region(dev_number,1);

    return -1;
}

static void __exit hello_world_exit(void)
{
    device_destroy(cls, dev_number);
    class_destroy(cls);
    cdev_del(&char_device);
    unregister_chrdev_region(dev_number, count);
    kfree(kern_data);

    printk("Device Destroyed,  Class Destroyed, Dev unregistered, cdev removed from system \n");
    printk("By By World \n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_AUTHOR("Nakul Chauhan");
MODULE_LICENSE("GPL");