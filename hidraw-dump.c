
/*
 * hidraw-dump -- Use Linux HIDRAW to print info about HID devices (USB & Bluetooth/BLE)
 *
 * 2021 - @todbot / Tod Kurt
 * https://github.com/todbot/hidraw-dump
 *
 * Based heavily on "hidraw-example.c":
 *  https://github.com/torvalds/linux/blob/master/samples/hidraw/hid-example.c
 * and "hidapi/linux/hid.c":
 *  https://github.com/libusb/hidapi/blob/master/linux/hid.c
 *
 */

/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include <libudev.h>
/*
 * Ugly hack to work around failing compilation on systems that don't
 * yet populate new version of hidraw.h to userspace.
 */
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif
/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

const char *bus_str(int bus);
int print_hidraw_info(const char* dev_path);

int main(int argc, char **argv)
{
    
    struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		fprintf(stderr,"Couldn't create udev context");
		return -1;
	}
	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "hidraw");
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);

	udev_list_entry_foreach(dev_list_entry, devices) {
		/* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
		const char *sysfs_path = udev_list_entry_get_name(dev_list_entry);
		struct udev_device *raw_dev; /* The device's hidraw udev node. */
        raw_dev = udev_device_new_from_syspath(udev, sysfs_path);
        const char *dev_path = udev_device_get_devnode(raw_dev);

        printf("---\n");
        printf("Device Path: %s\n", dev_path);
        
        print_hidraw_info( dev_path );
        
        udev_device_unref(raw_dev);
    }
    /* Free the enumerator and udev objects. */
	udev_enumerate_unref(enumerate);
	udev_unref(udev);

}

int print_hidraw_info(const char* dev_path)
{
	int fd;
	int i, res, desc_size = 0;
	char buf[256];
	struct hidraw_report_descriptor rpt_desc;
	struct hidraw_devinfo info;
    
	/* Open the Device with non-blocking reads. In real life,
	   don't use a hard coded path; use libudev instead. */
	fd = open( dev_path, O_RDWR|O_NONBLOCK);
	if (fd < 0) {
		perror("Unable to open device");
		return 1;
	}

    /* Get Raw Name */
	res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWNAME");
	else
		printf("Raw Name: %s\n", buf);
    
	/* Get Physical Location */
	res = ioctl(fd, HIDIOCGRAWPHYS(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWPHYS");
	else
		printf("Raw Phys: %s\n", buf);
    
	/* Get Raw Info */
	res = ioctl(fd, HIDIOCGRAWINFO, &info);
	if (res < 0) {
		perror("HIDIOCGRAWINFO");
	} else {
		printf("Raw Info:\n");
		printf("\tbustype: %d (%s)\n",
			info.bustype, bus_str(info.bustype));
		printf("\tvendor: 0x%04hx\n", info.vendor);
		printf("\tproduct: 0x%04hx\n", info.product);
	}

	memset(&rpt_desc, 0x0, sizeof(rpt_desc));
	memset(&info, 0x0, sizeof(info));
	memset(buf, 0x0, sizeof(buf));
	/* Get Report Descriptor Size */
	res = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
	if (res < 0)
		perror("HIDIOCGRDESCSIZE");
	else
		printf("Report Descriptor Size: %d\n", desc_size);
	/* Get Report Descriptor */
	rpt_desc.size = desc_size;
	res = ioctl(fd, HIDIOCGRDESC, &rpt_desc);
	if (res < 0) {
		perror("HIDIOCGRDESC");
	} else {
		printf("Report Descriptor:\n");
		for (i = 0; i < rpt_desc.size; i++) {
			//printf("%0hhx ", rpt_desc.value[i]);
			printf("%02X ", rpt_desc.value[i]);
            if(i % 16 == 15 ) { printf("\n"); }
        }
		puts("\n");
	}
    
	close(fd);
	return 0;
}
const char *
bus_str(int bus)
{
	switch (bus) {
	case BUS_USB:
		return "USB";
		break;
	case BUS_HIL:
		return "HIL";
		break;
	case BUS_BLUETOOTH:
		return "Bluetooth";
		break;
	case BUS_VIRTUAL:
		return "Virtual";
		break;
	default:
		return "Other";
		break;
	}
}
