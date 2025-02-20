#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/D5_device_ioctl"
#define MAGIC_NUM 100
#define IOCTL_CLEAR_BUFFER _IO(MAGIC_NUM, 1)
#define IOCTL_SET_VALUE    _IOW(MAGIC_NUM, 2, int)
#define IOCTL_GET_VALUE    _IOR(MAGIC_NUM, 3, int)

int main(){
    int fd, value = 42, received_value;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("Cannot open device");
        return -1;
    }

    printf("Clearing buffer...\n");
    ioctl(fd, IOCTL_CLEAR_BUFFER);

    printf("Getting value from driver...\n");
    ioctl(fd, IOCTL_GET_VALUE, &received_value);
    printf("initial value: %d\n", received_value);

    printf("Setting value to %d...\n", value);
    ioctl(fd, IOCTL_SET_VALUE, &value);

    printf("Getting value from driver...\n");
    ioctl(fd, IOCTL_GET_VALUE, &received_value);
    printf("Received value: %d\n", received_value);

    close(fd);
    return 0;
}