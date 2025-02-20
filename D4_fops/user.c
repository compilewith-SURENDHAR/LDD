#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/f_device"

int main()
{
    int fd;
    char write_buf[] = "hii kernel! this mesaage is from the user!";
    char read_buf[100];

    /* Open device file */
    fd = open(DEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("Cannot open device file");
        return -1;
    }

    /* Write data */
    printf("Writing data: %s\n", write_buf);
    write(fd, write_buf, strlen(write_buf));

    /* Read data */
    lseek(fd, 0, SEEK_SET);  // Reset file pointer
    read(fd, read_buf, sizeof(read_buf));
    printf("Read data: %s\n", read_buf);

    /* Close device */
    close(fd);
    return 0;
}
