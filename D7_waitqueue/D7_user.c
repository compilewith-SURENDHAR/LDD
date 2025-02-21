#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define DEVICE "/dev/waitqueue_device"
#define BUF_SIZE 80

void *read_thread(void *arg) {
    int fd;
    char buffer[BUF_SIZE];

    // Open device for reading
    fd = open(DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device for reading");
        return NULL;
    }

    printf("[READ THREAD] Waiting for data...\n");

    // Read from device (blocks until data is available)
    ssize_t bytes_read = read(fd, buffer, BUF_SIZE);
    if (bytes_read < 0) {
        perror("Read failed");
    } else {
        buffer[bytes_read] = '\0';  // Null-terminate the string
        printf("[READ THREAD] Data received: %s\n", buffer);
    }

    close(fd);
    return NULL;
}

int main() {
    int fd;
    char input[BUF_SIZE];
    pthread_t thread;

    // Create a reader thread
    pthread_create(&thread, NULL, read_thread, NULL);
    sleep(2);  // Give some time for the read to block

    // Open device for writing
    fd = open(DEVICE, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open device for writing");
        return 1;
    }

    printf("[MAIN] Enter data to write: ");
    fgets(input, BUF_SIZE, stdin);
    input[strcspn(input, "\n")] = 0;  // Remove newline

    // Write data to device (this wakes up the reader)
    if (write(fd, input, strlen(input)) < 0) {
        perror("Write failed");
    } else {
        printf("[MAIN] Data written successfully!\n");
    }

    close(fd);
    pthread_join(thread, NULL);  // Wait for reader thread to finish

    return 0;
}
