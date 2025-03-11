#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

#define SIGETX 44
#define REG_CURRENT_TASK _IOW('a', 'a', int32_t*)

void signal_handler(int signo, siginfo_t *info, void *extra) {
    printf("Received signal %d with value %d\n", signo, info->si_int);
}

int main() {
    struct sigaction act;
    int fd;

    // Register signal handler
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = signal_handler;
    sigaction(SIGETX, &act, NULL);

    // Open device file
    fd = open("/dev/etx_device", O_WRONLY);
    if (fd < 0) {
        printf("Failed to open device\n");
        return -1;
    }

    // Register process with driver
    ioctl(fd, REG_CURRENT_TASK, NULL);
    printf("Process registered with kernel. Waiting for signal...\n");

    
    // Write to device (triggers signal)
    write(fd, "1", 1);
    sleep(5);

    close(fd);
    return 0;
}
