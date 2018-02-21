#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>

#include <asm/uaccess.h>


// *** Everything below is a modification of the hello_dev.c which was given as example in the project 5 prompt

unsigned char get_random_byte(int max) {
        unsigned char c;
        get_random_bytes(&c, 1);
        return c%max;
}

static ssize_t dice_driver_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)
{
	int num_bytes = 1; // will contain the number of bytes to generate, for our purposes, this is usually going to be two, but could be any number of bytes as long as the buffer has space
	
	// Loop will continue until the number of bytes produced matches the requested number of bytes plus 1 (This is so the returned number of bytes is correct, not off by one)
	if(count == 0){
		return 0;
	}
	else{
		while(num_bytes < count+1){
			char num = get_random_byte(6); // random byte is generated
			put_user(num, buf++); // byte is transfered into the user space buffer, stored starting at index 0 of the buffer
			num_bytes++; // Increment the number of bytes produced thus far
		}
		return num_bytes; // Return the number of bytes produced, should always match the number of bytes requested
	}
}

/*
 * The only file operation we care about is read.
 */

static const struct file_operations dice_driver_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_driver_read,
};

static struct miscdevice dice_driver_dev = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/dice.
	 */
	"dice",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&dice_driver_fops
};

static int __init
dice_driver_init(void)
{
	int ret;

	/*
	 * Create the "dice_driver" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/dice_driver device using
	 * the default rules.
	 */
	ret = misc_register(&dice_driver_dev);
	if (ret)
		printk(KERN_ERR
		       "Unable to register dice_driver misc device\n");

	return ret;
}

module_init(dice_driver_init);

static void __exit
dice_driver_exit(void)
{
	misc_deregister(&dice_driver_dev);
}

module_exit(dice_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Addison Beall");
MODULE_DESCRIPTION("Dice roll simulator");
MODULE_VERSION("dev");
