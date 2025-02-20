#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>

#define WR_STRING _IOW('a', 'a', char *)  // Write string command
#define RD_STRING _IOR('a', 'b', char *)  // Read string command


int main() {
    int fd;
    char value[100];
    char read_buf[100];  // Buffer to store proc file content

    // Open the device file
    fd = open("/dev/etx_device", O_RDWR);
    if (fd < 0) {
        perror("Cannot open device file");
        return -1;
    }

    // Write a value via IOCTL
    printf("Enter a value to write: ");
    fgets(value, sizeof(value), stdin);
    value[strcspn(value, "\n")] = '\0';  // Remove trailing newline

    if (ioctl(fd, WR_STRING, value) < 0) {
        perror("IOCTL Write failed");
        close(fd);
        return -1;
    }
    printf("Written value: %s\n", value);

    // Read back from procfs via IOCTL
    memset(read_buf, 0, sizeof(read_buf));  // Clear buffer
    if (ioctl(fd, RD_STRING, read_buf) < 0) {
        perror("IOCTL Read failed");
        close(fd);
        return -1;
    }
    printf("Proc file content: %s\n", read_buf);

    // Close the device file
    close(fd);
    return 0;
}
