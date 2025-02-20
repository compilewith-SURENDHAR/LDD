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
    return 0;
}