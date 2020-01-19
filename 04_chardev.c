#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#define  DEVICE_NAME "04_chardev"    ///< The device will appear at /dev/ using this value
#define  CLASS_NAME  "cchdev"        ///< The device class -- this is a character device driver

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  my_deviceClass  = NULL; ///< The device-driver class struct pointer
static struct device* my_deviceDevice = NULL; ///< The device-driver device struct pointer

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};


static int __init my_device_init(void){
   printk(KERN_INFO "04_chardev Initializing the 04_chardev\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "my_device failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "04_chardev registered correctly with major number %d\n", majorNumber);

   // Register the device class
   my_deviceClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(my_deviceClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(my_deviceClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "04_chardev device class registered correctly\n");

   // Register the device driver
   my_deviceDevice = device_create(my_deviceClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(my_deviceDevice)){               // Clean up if there is an error
      class_destroy(my_deviceClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(my_deviceDevice);
   }
   printk(KERN_INFO "04_chardev device class created correctly\n"); // Made it! device was initialized
   return 0;
}


static void __exit my_device_exit(void){
   device_destroy(my_deviceClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(my_deviceClass);                          // unregister the device class
   class_destroy(my_deviceClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "04_chardev Goodbye from the LKM!\n");
}


static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "04_chardev Device has been opened %d time(s)\n", numberOpens);
   return 0;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "04_chardev Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "04_chardev Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}


static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "04_chardev Received %zu characters from the user\n", len);
   printk(KERN_INFO "04_chardev Received %d  and it multipied by 1024 is %d\n", message[0],message[0]*1024);
   return len;
}


static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "04_chardev Device successfully closed\n");
   return 0;
}


module_init(my_device_init);
module_exit(my_device_exit);