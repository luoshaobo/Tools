#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <libudev.h>

////////////////////////////////////////////////////////////////////////////////////
// a sample of the file /home/root/on_usb_mounted.sh
//
#if 0
#!/bin/bash

DEV_FILE="$1"

if [ -z "$DEV_FILE" ]; then
    #echo "*** Error: \$DEV_FILE is NULL" >&2
    exit 0
fi

MOUNT_POINT=`busybox mount | grep "$DEV_FILE on " | awk '{print $3}'`
if [ -z "$MOUNT_POINT" ]; then
    #echo "*** Error: \$MOUNT_POINT is NULL" >&2
    exit 0
fi

echo " MOUNT_POINT=$MOUNT_POINT"

if [ ! -f "$MOUNT_POINT/12345678.tag" ]; then
    #echo "*** Error: \$MOUNT_POINT/12345678.tag does not exist" >&2
    exit 0
fi

busybox mount -o remount,rw $DEV_FILE
touch $MOUNT_POINT/haha.txt
sync
#endif // #if 0

void execute_script(const char *dev_node)
{
    char sCmd[1024] = {0};
    snprintf(sCmd, 1024, "/bin/sh /home/root/on_usb_mounted.sh %s", dev_node);
    system(sCmd);
}

int main2(int argc, char* argv[])
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;
    struct udev_monitor *mon;
    int fd;

    /* Create the udev object */
    udev = udev_new();
    if (!udev) {
        printf("Can't create udev\n");
        exit(1);
    }

    /* This section sets up a monitor which will report events when
     devices attached to the system change. Events include "add",
     "remove", "change", "online", and "offline".

     This section sets up and starts the monitoring. Events are
     polled for (and delivered) later in the file.

     It is important that the monitor be set up before the call to
     udev_enumerate_scan_devices() so that events (and devices) are
     not missed. For example, if enumeration happened first, there
     would be no event generated for a device which was attached after
     enumeration but before monitoring began.

     Note that a filter is added so that we only get events for
     "hidraw" devices. */

    /* Set up a monitor to monitor hidraw devices */
    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);    // matched by "block"
    udev_monitor_enable_receiving(mon);
    /* Get the file descriptor (fd) for the monitor.
     This fd will get passed to select() */
    fd = udev_monitor_get_fd(mon);


    /* Create a list of the devices in the 'hidraw' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "block");                 // matched by "block"
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    /* For each item enumerated, print out its information.
     udev_list_entry_foreach is a macro which expands to
     a loop. The loop will be executed for each member in
     devices, setting dev_list_entry to a list entry
     which contains the device's path in /sys. */
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;

        /* Get the filename of the /sys entry for the device
         and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        /* usb_device_get_devnode() returns the path to the device node
         itself in /dev. */
        printf("Device Node Path: %s\n", udev_device_get_devnode(dev));

        /* The device pointed to by dev contains information about
         the hidraw device. In order to get information about the
         USB device, get the parent device with the
         subsystem/devtype pair of "usb"/"usb_device". This will
         be several levels up the tree, but the function will find
         it.*/
        dev = udev_device_get_parent_with_subsystem_devtype(
         dev,
         "usb",
         "usb_device");
        if (!dev) {
            printf("Unable to find parent usb device.\n\n");
            continue;
        }

        /* From here, we can call get_sysattr_value() for each file
         in the device's /sys entry. The strings passed into these
         functions (idProduct, idVendor, serial, etc.) correspond
         directly to the files in the /sys directory which
         represents the USB device. Note that USB strings are
         Unicode, UCS2 encoded, but the strings returned from
         udev_device_get_sysattr_value() are UTF-8 encoded. */
        printf(" VID/PID: %s %s\n",
         udev_device_get_sysattr_value(dev,"idVendor"),
         udev_device_get_sysattr_value(dev, "idProduct"));
        printf(" %s\n %s\n",
         udev_device_get_sysattr_value(dev,"manufacturer"),
         udev_device_get_sysattr_value(dev,"product"));
        printf(" serial: %s\n",
         udev_device_get_sysattr_value(dev, "serial"));
        udev_device_unref(dev);
        printf("\n");
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);

    /* Begin polling for udev events. Events occur when devices
     attached to the system are added, removed, or change state.
     udev_monitor_receive_device() will return a device
     object representing the device which changed and what type of
     change occured.

     The select() system call is used to ensure that the call to
     udev_monitor_receive_device() will not block.

     The monitor was set up earler in this file, and monitoring is
     already underway.

     This section will run continuously, calling usleep() at the end
     of each pass. This is to demonstrate how to use a udev_monitor
     in a non-blocking way. */
    while (1) {
        /* Set up the call to select(). In this case, select() will
         only operate on a single file descriptor, the one
         associated with our udev_monitor. Note that the timeval
         object is set to 0, which will cause select() to not
         block. */
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        ret = select(fd+1, &fds, NULL, NULL, &tv);

        /* Check if our file descriptor has received data. */
        if (ret > 0 && FD_ISSET(fd, &fds)) {
            printf("\nselect() says there should be data\n");

            /* Make the call to receive the device.
             select() ensured that this will not block. */
            dev = udev_monitor_receive_device(mon);
            if (dev) {
                printf("Got Device\n");
                printf(" Node: %s\n", udev_device_get_devnode(dev));
                execute_script(udev_device_get_devnode(dev));
                printf(" Subsystem: %s\n", udev_device_get_subsystem(dev));
                printf(" Devtype: %s\n", udev_device_get_devtype(dev));

                printf(" Action: %s\n", udev_device_get_action(dev));
                udev_device_unref(dev);
            }
            else {
                printf("No Device from receive_device(). An error occured.\n");
            }

            printf("\n");
            fflush(stdout);
        }
    }
    udev_unref(udev);

    return 0;
}
